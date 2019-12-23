#include "test.hpp"

#include "writer_string.hpp"
#include "allocator.hpp"

#include <string>
#include <vector>

using namespace pugi;

namespace
{
	int page_allocs = 0;
	int page_deallocs = 0;

	size_t memory_allocated = 0;

	bool is_page(size_t size)
	{
		return size >= 16384;
	}

	void* allocate(size_t size)
	{
		void* ptr = memory_allocate(size);
		page_allocs += is_page(memory_size(ptr));
		return ptr;
	}

	void deallocate(void* ptr)
	{
		page_deallocs += is_page(memory_size(ptr));
		memory_deallocate(ptr);
	}

	void* null_allocate(size_t)
	{
#ifndef PUGIXML_NO_EXCEPTIONS
		throw std::runtime_error("null_allocate was invoked");
#else
		return 0;
#endif
	}

	void null_deallocate(void*)
	{
#ifndef PUGIXML_NO_EXCEPTIONS
		throw std::runtime_error("null_deallocate was invoked");
#endif
	}

	class test_memory_pool : public pugi::xml_memory_pool
	{
	public:

		test_memory_pool(pugi::allocation_function alloc, pugi::deallocation_function dealloc) PUGIXML_NOEXCEPT
			: _allocate(alloc)
			, _deallocate(dealloc)
		{
		}

		void* allocate(size_t size) PUGIXML_OVERRIDE
		{
			return _allocate(size);
		}

		void deallocate(void* ptr) PUGIXML_OVERRIDE
		{
			_deallocate(ptr);
		}

	private:
		pugi::allocation_function _allocate;
		pugi::deallocation_function _deallocate;
	};

	// class with interface similar to std::pmr::memory_resource
	class test_memory_resource
	{
	public:

		test_memory_resource(pugi::allocation_function alloc, pugi::deallocation_function dealloc) PUGIXML_NOEXCEPT
			: _allocate(alloc)
			, _deallocate(dealloc)
		{
		}

		void* allocate(size_t size, size_t alignment)
		{
			if (alignment != pugi::memory_allocation_alignment)
			{
			#ifndef PUGIXML_NO_EXCEPTIONS
				throw std::runtime_error("Invalid alignment during allocation");
			#else
				return 0;
			#endif
			}

			memory_allocated += size;
			return _allocate(size);
		}

		void deallocate(void* ptr, size_t size, size_t alignment)
		{
			if (alignment != pugi::memory_allocation_alignment)
			{
			#ifndef PUGIXML_NO_EXCEPTIONS
				throw std::runtime_error("Invalid alignment during deallocation");
			#else
				return;
			#endif
			}

			memory_allocated -= size;
			_deallocate(ptr);
		}

	private:
		pugi::allocation_function _allocate;
		pugi::deallocation_function _deallocate;
	};
}

TEST(memory_custom_memory_management)
{
	page_allocs = page_deallocs = 0;

	// remember old functions
	allocation_function old_allocate = get_memory_allocation_function();
	deallocation_function old_deallocate = get_memory_deallocation_function();

	// replace functions
	set_memory_management_functions(allocate, deallocate);

	{
		// parse document
		xml_document doc;

		CHECK(page_allocs == 0 && page_deallocs == 0);

		CHECK(doc.load_string(STR("<node />")));

		CHECK(page_allocs == 1 && page_deallocs == 0);

		// modify document (no new page)
		CHECK(doc.first_child().set_name(STR("foobars")));
		CHECK(page_allocs == 1 && page_deallocs == 0);

		// modify document (new page)
		std::basic_string<char_t> s(65536, 'x');

		CHECK(doc.first_child().set_name(s.c_str()));
		CHECK(page_allocs == 2 && page_deallocs == 0);

		// modify document (new page, old one should die)
		s += s;

		CHECK(doc.first_child().set_name(s.c_str()));
		CHECK(page_allocs == 3 && page_deallocs == 1);
	}

	CHECK(page_allocs == 3 && page_deallocs == 3);

	// restore old functions
	set_memory_management_functions(old_allocate, old_deallocate);
}

TEST(memory_pool_custom_memory_management)
{
	page_allocs = page_deallocs = 0;

	// remember old functions
	allocation_function old_allocate = get_memory_allocation_function();
	deallocation_function old_deallocate = get_memory_deallocation_function();

	// replace functions with non-allocating ones
	set_memory_management_functions(null_allocate, null_deallocate);

	{
		// create custom memory pool
		test_memory_pool pool(allocate, deallocate);

		// parse document
		xml_document doc(pool);

		CHECK(page_allocs == 0 && page_deallocs == 0);

		CHECK(doc.load_string(STR("<node />")));

		CHECK(page_allocs == 1 && page_deallocs == 0);

		// modify document (no new page)
		CHECK(doc.first_child().set_name(STR("foobars")));
		CHECK(page_allocs == 1 && page_deallocs == 0);

		// modify document (new page)
		std::basic_string<char_t> s(65536, 'x');

		CHECK(doc.first_child().set_name(s.c_str()));
		CHECK(page_allocs == 2 && page_deallocs == 0);

		// modify document (new page, old one should die)
		s += s;

		CHECK(doc.first_child().set_name(s.c_str()));
		CHECK(page_allocs == 3 && page_deallocs == 1);
	}

	CHECK(page_allocs == 3 && page_deallocs == 3);

	// restore old functions
	set_memory_management_functions(old_allocate, old_deallocate);
}

TEST(memory_large_allocations)
{
	page_allocs = page_deallocs = 0;

	// remember old functions
	allocation_function old_allocate = get_memory_allocation_function();
	deallocation_function old_deallocate = get_memory_deallocation_function();

	// replace functions
	set_memory_management_functions(allocate, deallocate);

	{
		xml_document doc;

		CHECK(page_allocs == 0 && page_deallocs == 0);

		// initial fill
		for (size_t i = 0; i < 128; ++i)
		{
			std::basic_string<char_t> s(i * 128, 'x');

			CHECK(doc.append_child(node_pcdata).set_value(s.c_str()));
		}

		CHECK(page_allocs > 0 && page_deallocs == 0);

		// grow-prune loop
		while (doc.first_child())
		{
			xml_node node;

			// grow
			for (node = doc.first_child(); node; node = node.next_sibling())
			{
				std::basic_string<char_t> s = node.value();

				CHECK(node.set_value((s + s).c_str()));
			}

			// prune
			for (node = doc.first_child(); node; )
			{
				xml_node next = node.next_sibling().next_sibling();

				node.parent().remove_child(node);

				node = next;
			}
		}

		CHECK(page_allocs == page_deallocs + 1); // only one live page left (it waits for new allocations)

		char buffer;
		CHECK(doc.load_buffer_inplace(&buffer, 0, parse_fragment, get_native_encoding()));

		CHECK(page_allocs == page_deallocs); // no live pages left
	}

	CHECK(page_allocs == page_deallocs); // everything is freed

	// restore old functions
	set_memory_management_functions(old_allocate, old_deallocate);
}

TEST(memory_pool_large_allocations)
{
	page_allocs = page_deallocs = 0;

	// remember old functions
	allocation_function old_allocate = get_memory_allocation_function();
	deallocation_function old_deallocate = get_memory_deallocation_function();

	// replace functions with non-allocating ones
	set_memory_management_functions(null_allocate, null_deallocate);

	{
		// create custom memory pool
		test_memory_pool pool(allocate, deallocate);

		xml_document doc(pool);

		CHECK(page_allocs == 0 && page_deallocs == 0);

		// initial fill
		for (size_t i = 0; i < 128; ++i)
		{
			std::basic_string<char_t> s(i * 128, 'x');

			CHECK(doc.append_child(node_pcdata).set_value(s.c_str()));
		}

		CHECK(page_allocs > 0 && page_deallocs == 0);

		// grow-prune loop
		while (doc.first_child())
		{
			xml_node node;

			// grow
			for (node = doc.first_child(); node; node = node.next_sibling())
			{
				std::basic_string<char_t> s = node.value();

				CHECK(node.set_value((s + s).c_str()));
			}

			// prune
			for (node = doc.first_child(); node; )
			{
				xml_node next = node.next_sibling().next_sibling();

				node.parent().remove_child(node);

				node = next;
			}
		}

		CHECK(page_allocs == page_deallocs + 1); // only one live page left (it waits for new allocations)

		char buffer;
		CHECK(doc.load_buffer_inplace(&buffer, 0, parse_fragment, get_native_encoding()));

		CHECK(page_allocs == page_deallocs); // no live pages left
	}

	CHECK(page_allocs == page_deallocs); // everything is freed

	// restore old functions
	set_memory_management_functions(old_allocate, old_deallocate);
}

TEST(memory_page_management)
{
	page_allocs = page_deallocs = 0;

	// remember old functions
	allocation_function old_allocate = get_memory_allocation_function();
	deallocation_function old_deallocate = get_memory_deallocation_function();

	// replace functions
	set_memory_management_functions(allocate, deallocate);

	{
		xml_document doc;

		CHECK(page_allocs == 0 && page_deallocs == 0);

		// initial fill
		std::vector<xml_node> nodes;

		for (size_t i = 0; i < 4000; ++i)
		{
			xml_node node = doc.append_child(STR("n"));
			CHECK(node);

			nodes.push_back(node);
		}

		CHECK(page_allocs > 0 && page_deallocs == 0);

		// grow-prune loop
		size_t offset = 0;
		size_t prime = 15485863;

		while (nodes.size() > 0)
		{
			offset = (offset + prime) % nodes.size();

			doc.remove_child(nodes[offset]);

			nodes[offset] = nodes.back();
			nodes.pop_back();
		}

		CHECK(page_allocs == page_deallocs + 1); // only one live page left (it waits for new allocations)

		char buffer;
		CHECK(doc.load_buffer_inplace(&buffer, 0, parse_fragment, get_native_encoding()));

		CHECK(page_allocs == page_deallocs); // no live pages left
	}

	CHECK(page_allocs == page_deallocs); // everything is freed

	// restore old functions
	set_memory_management_functions(old_allocate, old_deallocate);
}

TEST(memory_resource_page_management)
{
	page_allocs = page_deallocs = 0;

	// remember old functions
	allocation_function old_allocate = get_memory_allocation_function();
	deallocation_function old_deallocate = get_memory_deallocation_function();

	// replace functions with non-allocating ones
	set_memory_management_functions(null_allocate, null_deallocate);

	{
		// create custom memory pool
		test_memory_pool pool(allocate, deallocate);

		xml_document doc(pool);

		CHECK(page_allocs == 0 && page_deallocs == 0);

		// initial fill
		std::vector<xml_node> nodes;

		for (size_t i = 0; i < 4000; ++i)
		{
			xml_node node = doc.append_child(STR("n"));
			CHECK(node);

			nodes.push_back(node);
		}

		CHECK(page_allocs > 0 && page_deallocs == 0);

		// grow-prune loop
		size_t offset = 0;
		size_t prime = 15485863;

		while (nodes.size() > 0)
		{
			offset = (offset + prime) % nodes.size();

			doc.remove_child(nodes[offset]);

			nodes[offset] = nodes.back();
			nodes.pop_back();
		}

		CHECK(page_allocs == page_deallocs + 1); // only one live page left (it waits for new allocations)

		char buffer;
		CHECK(doc.load_buffer_inplace(&buffer, 0, parse_fragment, get_native_encoding()));

		CHECK(page_allocs == page_deallocs); // no live pages left
	}

	CHECK(page_allocs == page_deallocs); // everything is freed

	// restore old functions
	set_memory_management_functions(old_allocate, old_deallocate);
}

TEST(memory_string_allocate_increasing)
{
	xml_document doc;

	doc.append_child(node_pcdata).set_value(STR("x"));

	std::basic_string<char_t> s = STR("ab");

	for (int i = 0; i < 17; ++i)
	{
		doc.append_child(node_pcdata).set_value(s.c_str());

		s += s;
	}

	std::string result = save_narrow(doc, format_no_declaration | format_raw, encoding_utf8);

	CHECK(result.size() == 262143);
	CHECK(result[0] == 'x');

	for (size_t j = 1; j < result.size(); ++j)
	{
		CHECK(result[j] == (j % 2 ? 'a' : 'b'));
	}
}

TEST(memory_string_allocate_decreasing)
{
	xml_document doc;

	std::basic_string<char_t> s = STR("ab");

	for (int i = 0; i < 17; ++i) s += s;

	for (int j = 0; j < 17; ++j)
	{
		s.resize(s.size() / 2);

		doc.append_child(node_pcdata).set_value(s.c_str());
	}

	doc.append_child(node_pcdata).set_value(STR("x"));

	std::string result = save_narrow(doc, format_no_declaration | format_raw, encoding_utf8);

	CHECK(result.size() == 262143);
	CHECK(result[result.size() - 1] == 'x');

	for (size_t k = 0; k + 1 < result.size(); ++k)
	{
		CHECK(result[k] == (k % 2 ? 'b' : 'a'));
	}
}

TEST(memory_string_allocate_increasing_inplace)
{
	xml_document doc;

	xml_node node = doc.append_child(node_pcdata);

	node.set_value(STR("x"));

	std::basic_string<char_t> s = STR("ab");

	for (int i = 0; i < 17; ++i)
	{
		node.set_value(s.c_str());

		s += s;
	}

	std::string result = save_narrow(doc, format_no_declaration | format_raw, encoding_utf8);

	CHECK(result.size() == 131072);

	for (size_t j = 0; j < result.size(); ++j)
	{
		CHECK(result[j] == (j % 2 ? 'b' : 'a'));
	}
}

TEST(memory_string_allocate_decreasing_inplace)
{
	xml_document doc;

	xml_node node = doc.append_child(node_pcdata);

	std::basic_string<char_t> s = STR("ab");

	for (int i = 0; i < 17; ++i) s += s;

	for (int j = 0; j < 17; ++j)
	{
		s.resize(s.size() / 2);

		node.set_value(s.c_str());
	}

	node.set_value(STR("x"));

	std::string result = save_narrow(doc, format_no_declaration | format_raw, encoding_utf8);

	CHECK(result == "x");
}

TEST(monotonic_memory_pool_management)
{
	page_allocs = page_deallocs = 0;

	// remember old functions
	allocation_function old_allocate = get_memory_allocation_function();
	deallocation_function old_deallocate = get_memory_deallocation_function();

	// replace functions with non-allocating ones
	set_memory_management_functions(null_allocate, null_deallocate);

	const size_t buffer_size = 64 * 1024;
	void* buffer = old_allocate(buffer_size);

	{
		// create custom memory pool
		xml_monotonic_memory_pool pool(buffer, buffer_size);

		// parse document
		xml_document doc(pool);

		CHECK(page_allocs == 0 && page_deallocs == 0);

		// string length should be odd number in order to detect any memory misalighment issues
		CHECK(doc.load_string(STR("<node><description>Simple node</description><param name='id' value='123'/>1</node>")));

		CHECK(page_allocs == 0 && page_deallocs == 0);
	}

	old_deallocate(buffer);

	// restore old functions
	set_memory_management_functions(old_allocate, old_deallocate);
}

TEST(monotonic_memory_pool_allocation)
{
	// make address not aligned on 8 byte boundary
	void* buffer = reinterpret_cast<void*>(pugi::memory_allocation_alignment + 3);
	size_t size = 20;

	xml_monotonic_memory_pool pool(buffer, size);
	// buffer should start at 8 byte boundary, minimum allocation is 8 bytes
	void* result = pool.allocate(6);
	CHECK(result == reinterpret_cast<void*>(16));

	// now capacity should not be enough for memory allocation
	result = pool.allocate(12);
	CHECK(result == 0);
}

TEST(memory_resource_pool_management)
{
	memory_allocated = 0;

	// remember old functions
	allocation_function old_allocate = get_memory_allocation_function();
	deallocation_function old_deallocate = get_memory_deallocation_function();

	// replace functions with non-allocating ones
	set_memory_management_functions(null_allocate, null_deallocate);

	// create custom memory pool
	test_memory_resource resource(old_allocate, old_deallocate);
	memory_resource_adapter<test_memory_resource> pool(&resource);

	{
		// parse document
		xml_document doc(pool);

		CHECK(memory_allocated == 0);

		CHECK(doc.load_string(STR("<node />")));

		CHECK(memory_allocated != 0);

		// modify document (no new page)
		CHECK(doc.first_child().set_name(STR("foobars")));

		// modify document (new page)
		std::basic_string<char_t> s(65536, 'x');

		CHECK(doc.first_child().set_name(s.c_str()));

		// modify document (new page, old one should die)
		s += s;

		CHECK(doc.first_child().set_name(s.c_str()));
	}

	CHECK(memory_allocated == 0);

	// restore old functions
	set_memory_management_functions(old_allocate, old_deallocate);
}

#ifdef PUGIXML_HAS_MOVE
TEST(memory_pool_swap)
{
	memory_allocated = 0;

	// remember old functions
	allocation_function current_allocate = get_memory_allocation_function();
	deallocation_function current_deallocate = get_memory_deallocation_function();

	{
		// create custom memory pool
		test_memory_resource resource(current_allocate, current_deallocate);
		memory_resource_adapter<test_memory_resource> pool(&resource);

		// parse document
		xml_document doc_with_custom_pool(pool);
		xml_document doc_with_default_pool;

		CHECK(memory_allocated == 0);

		CHECK(doc_with_custom_pool.load_string(STR("<node />")));
		CHECK(doc_with_custom_pool.first_child().set_name(STR("foobars")));

		CHECK(memory_allocated != 0);

		// move content of the document with custom memory pool
		doc_with_default_pool = std::move(doc_with_custom_pool);

		// memory from custom pool should become deallocated
		// which means that memory pool wasn't swapped
		// and that document is now empty
		CHECK(memory_allocated == 0);

		// check document with default pool
		xml_node node = doc_with_default_pool.first_child();
		CHECK_STRING(node.name(), STR("foobars"));
	}

	CHECK(memory_allocated == 0);
}
#endif

#ifndef PUGIXML_NO_XPATH
TEST(memory_pool_management_for_xpath_query)
{
	const char_t* paths = STR("(//location[string(info/references/reference[1]/url)=string(current-url)]/info/references/reference[1])[1]");

	char buffer[5 * 1024];

	xml_monotonic_memory_pool pool(buffer, sizeof(buffer));
	xpath_query q(pool, paths);
	CHECK(q.result());
}

#ifdef PUGIXML_HAS_MOVE
TEST(memory_pool_swap_for_xpath_query)
{
	memory_allocated = 0;

	// remember old functions
	allocation_function current_allocate = get_memory_allocation_function();
	deallocation_function current_deallocate = get_memory_deallocation_function();

	{
		// create custom memory pool
		test_memory_resource resource(current_allocate, current_deallocate);
		memory_resource_adapter<test_memory_resource> pool(&resource);

		const char_t* paths1 = STR("(//location[string(info/references/reference[1]/url)=string(current-url)]/info/references/reference[1])[1]");
		const char_t* paths2 = STR("/descendant-or-self::node()/child::para");

		xpath_query query_with_custom_pool(pool, paths1);
		size_t memory_allocated_for_query_with_custom_pool = memory_allocated;
		CHECK(memory_allocated_for_query_with_custom_pool != 0);

		xpath_query query_with_default_pool(paths2);
		CHECK(memory_allocated_for_query_with_custom_pool == memory_allocated);

		// query_with_custom_pool should receive default pool after the move
		query_with_custom_pool = std::move(query_with_default_pool);
		CHECK(memory_allocated == 0);
	}
}
#endif

#endif
