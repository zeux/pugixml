///////////////////////////////////////////////////////////////////////////////
//
// Pug Improved XML Parser - Version 0.1
// --------------------------------------------------------
// Copyright (C) 2006, by Arseny Kapoulkine (arseny.kapoulkine@gmail.com)
// This work is based on the pugxml parser, which is:
// Copyright (C) 2003, by Kristen Wegner (kristen@tima.net)
// Released into the Public Domain. Use at your own risk.
// See pugxml.xml for further information, history, etc.
// Contributions by Neville Franks (readonly@getsoft.com).
//
///////////////////////////////////////////////////////////////////////////////

#ifndef HEADER_PUGIXML_HPP
#define HEADER_PUGIXML_HPP

/// The PugiXML Parser namespace.
namespace pugi
{
	/// Tree node classification.
	/// See 'xml_node_struct::type'
	enum xml_node_type
	{
		node_null,			///< An undifferentiated entity.
		node_document,		///< A document tree's absolute root.
		node_element,		///< E.g. '<...>'
		node_pcdata,		///< E.g. '<...>'
		node_cdata,			///< E.g. '<![CDATA[...]]>'
		node_comment,		///< E.g. '<!--...-->'
		node_pi				///< E.g. '<?...?>'
	};

	/// Parser Options
	const size_t memory_block_size = 32768;		///< Memory block size, 32 kb

	const unsigned int parse_minimal			= 0x00000000; ///< Unset the following flags.
	const unsigned int parse_pi					= 0x00000001; ///< Parse '<?...?>'
	const unsigned int parse_comments			= 0x00000002; ///< Parse '<!--...-->'
	const unsigned int parse_cdata				= 0x00000004; ///< Parse '<![CDATA[...]]>'
	const unsigned int parse_ws_pcdata			= 0x00000008; ///< Skip PCDATA that consists only of whitespaces
	const unsigned int parse_ext_pcdata			= 0x00000010; ///< Skip PCDATA that is outside all tags (i.e. root)
	const unsigned int parse_trim_pcdata		= 0x00000020; ///< Trim '>...<'
	const unsigned int parse_trim_attribute		= 0x00000040; ///< Trim 'foo="..."'.
	const unsigned int parse_escapes_pcdata		= 0x00000080; ///< Parse &lt;, &gt;, &amp;, &quot;, &apos;, &#.. sequences
	const unsigned int parse_escapes_attribute 	= 0x00000100; ///< Parse &lt;, &gt;, &amp;, &quot;, &apos;, &#.. sequences
	const unsigned int parse_wnorm_pcdata		= 0x00000200; ///< Normalize spaces in pcdata
	const unsigned int parse_wnorm_attribute	= 0x00000400; ///< Normalize spaces in attributes
	const unsigned int parse_wconv_attribute	= 0x00000800; ///< Convert space-like characters to spaces in attributes (only if wnorm is not set)
	const unsigned int parse_eol_pcdata			= 0x00001000; ///< Perform EOL handling in pcdata
	const unsigned int parse_eol_attribute		= 0x00002000; ///< Perform EOL handling in attrobites
	const unsigned int parse_eol_cdata			= 0x00004000; ///< Perform EOL handling in CDATA sections
	const unsigned int parse_check_end_tags		= 0x00010000; ///< Check start and end tag names and return error if names mismatch
	const unsigned int parse_match_end_tags		= 0x00020000; ///< Try to find corresponding start tag for an end tag
	///< Set all flags, except parse_ws_pcdata and parse_trim_attribute
	const unsigned int parse_default			= 0x00FFFFFF & ~parse_ws_pcdata & ~parse_trim_attribute;
	const unsigned int parse_noset				= 0x80000000; ///< Parse with flags in xml_parser

	const unsigned int parse_w3c				= parse_pi | parse_comments | parse_cdata |
												parse_escapes_pcdata | parse_escapes_attribute |
												parse_wconv_attribute | parse_check_end_tags |
												parse_ws_pcdata | parse_eol_cdata;

	/// Forward declarations
	struct xml_attribute_struct;
	struct xml_node_struct;

	class xml_node_iterator;
	class xml_attribute_iterator;

	class xml_tree_walker;

	/// Provides a light-weight wrapper for manipulating xml_attribute_struct structures.
	///	Note: xml_attribute does not create any memory for the attribute it wraps; 
	///	it only wraps a pointer to an existing xml_attribute_struct.
	class xml_attribute
	{
		friend class xml_attribute_iterator;
		friend class xml_node;

	private:
		const xml_attribute_struct* _attr; ///< The internal attribute pointer.
	
    	/// Safe bool type
    	typedef const xml_attribute_struct* xml_attribute::*unspecified_bool_type;

		/// Initializing ctor
		explicit xml_attribute(const xml_attribute_struct* attr);

	public:
		/// Default ctor
		xml_attribute();
		
	public:
		/// Comparison operators
		bool operator==(const xml_attribute& r) const;
		bool operator!=(const xml_attribute& r) const;
		bool operator<(const xml_attribute& r) const;
		bool operator>(const xml_attribute& r) const;
		bool operator<=(const xml_attribute& r) const;
		bool operator>=(const xml_attribute& r) const;
	
    	/// Safe bool conversion
    	operator unspecified_bool_type() const;

    	/// Get next attribute if any, else xml_attribute()
    	xml_attribute next_attribute() const;

    	/// Get previous attribute if any, else xml_attribute()
    	xml_attribute previous_attribute() const;

		/// Cast attribute value as int. If not found, return 0.
		/// \return Attribute value as int, or 0.
		int as_int() const;

		/// Cast attribute value as double. If not found, return 0.0.
		/// \return Attribute value as double, or 0.0.
		double as_double() const;
	
		/// Cast attribute value as float. If not found, return 0.0.
		/// \return Attribute value as float, or 0.0.
		float as_float() const;

		/// Cast attribute value as bool. If not found, return false.
		/// \return Attribute value as bool, or false.
		bool as_bool() const;

	public:
		/// True if internal pointer is valid
		bool empty() const;

	public:
		/// Access the attribute name.
		const char* name() const;

		/// Access the attribute value.
		const char* value() const;
	};

	/// Provides a light-weight wrapper for manipulating xml_node_struct structures.
	class xml_node
	{
		friend class xml_node_iterator;
		friend class xml_parser;

	private:
		const xml_node_struct* _root; ///< Pointer to node root.

    	/// Safe bool type
    	typedef const xml_node_struct* xml_node::*unspecified_bool_type;

	private:
		/// Node is tree root.
		bool type_document() const;

	public:
		/// Default constructor.
		///	Node root points to a dummy 'xml_node_struct' structure. Test for this 
		///	with 'empty'.
		xml_node();

		/// Construct, wrapping the given 'xml_node_struct' pointer.
		explicit xml_node(const xml_node_struct* p);

	public:
		/// Base iterator type (for child nodes). Same as 'child_iterator'.
		typedef xml_node_iterator iterator;

		/// Attribute iterator type.
		typedef xml_attribute_iterator attribute_iterator;

		/// Access the begin iterator for this node's collection of child nodes.
		/// Same as 'children_begin'.
		iterator begin() const;
	
		/// Access the end iterator for this node's collection of child nodes.
		/// Same as 'children_end'.
		iterator end() const;
		
		/// Access the begin iterator for this node's collection of child nodes.
		/// Same as 'begin'.
		iterator children_begin() const;
	
		/// Access the end iterator for this node's collection of child nodes.
		/// Same as 'end'.
		iterator children_end() const;
	
		/// Access the begin iterator for this node's collection of attributes.
		attribute_iterator attributes_begin() const;
	
		/// Access the end iterator for this node's collection of attributes.
		attribute_iterator attributes_end() const;

		/// Access the begin iterator for this node's collection of siblings.
		iterator siblings_begin() const;
	
		/// Access the end iterator for this node's collection of siblings.
		iterator siblings_end() const;
	
	public:
    	/// Safe bool conversion
		operator unspecified_bool_type() const;
	
		/// Comparison operators
		bool operator==(const xml_node& r) const;
		bool operator!=(const xml_node& r) const;
		bool operator<(const xml_node& r) const;
		bool operator>(const xml_node& r) const;
		bool operator<=(const xml_node& r) const;
		bool operator>=(const xml_node& r) const;

	public:
		/// Node pointer is null, or type is node_null. Same as type_null.
		bool empty() const;

	public:
		/// Access node entity type.
		xml_node_type type() const;

		/// Access pointer to node name if any, else empty string.
		const char* name() const;

		/// Access pointer to data if any, else empty string.
		const char* value() const;
	
		/// Access child node at name as xml_node or xml_node(NULL) if bad name.
		xml_node child(const char* name) const;

		/// Access child node at name as xml_node or xml_node(NULL) if bad name.
		/// Enable wildcard matching.
		xml_node child_w(const char* name) const;

		/// Access the attribute having 'name'.
		xml_attribute attribute(const char* name) const;

		/// Access the attribute having 'name'.
		/// Enable wildcard matching.
		xml_attribute attribute_w(const char* name) const;

		/// Access sibling node at name as xml_node or xml_node(NULL) if bad name.
		xml_node sibling(const char* name) const;

		/// Access sibling node at name as xml_node or xml_node(NULL) if bad name.
		/// Enable wildcard matching.
		xml_node sibling_w(const char* name) const;

		/// Access current node's next sibling by position and name.
		xml_node next_sibling(const char* name) const;

		/// Access current node's next sibling by position and name.
		/// Enable wildcard matching.
		xml_node next_sibling_w(const char* name) const;

		/// Access current node's next sibling by position.
		xml_node next_sibling() const;

		/// Access current node's previous sibling by position and name.
		xml_node previous_sibling(const char* name) const;

		/// Access current node's previous sibling by position and name.
		/// Enable wildcard matching.
		xml_node previous_sibling_w(const char* name) const;

		/// Access current node's previous sibling by position.
		xml_node previous_sibling() const;

		/// Access node's parent if any, else xml_node(NULL)
		xml_node parent() const;

		/// Return PCDATA/CDATA that is child of current node. If none, return empty string.
		const char* child_value() const;

		/// Return PCDATA/CDATA that is child of specified child node. If none, return empty string.
		const char* child_value(const char* name) const;

		/// Return PCDATA/CDATA that is child of specified child node. If none, return empty string.
		/// Enable wildcard matching.
		const char* child_value_w(const char* name) const;

	public:
		/// Access node's first attribute if any, else xml_attribute()
		xml_attribute first_attribute() const;

		/// Access node's last attribute if any, else xml_attribute()
        xml_attribute last_attribute() const;

		/// Find all elements having the given name.
		template <typename OutputIterator> void all_elements_by_name(const char* name, OutputIterator it) const;

		/// Find all elements having the given name.
		/// Enable wildcard matching.
		template <typename OutputIterator> void all_elements_by_name_w(const char* name, OutputIterator it) const;

		/// Access node's first child if any, else xml_node()
		xml_node first_child() const;

		/// Access node's last child if any, else xml_node()
        xml_node last_child() const;
		
		/// Find attribute using the predicate
		/// Predicate should take xml_attribute and return bool.
		template <typename Predicate> xml_attribute find_attribute(Predicate pred) const;

		/// Find child using the predicate
		/// Predicate should take xml_node and return bool.
		template <typename Predicate> xml_node find_child(Predicate pred) const;

		/// Recursively-implemented depth-first find element using the predicate
		/// Predicate should take xml_node and return bool.
		template <typename Predicate> xml_node find_element(Predicate pred) const;

		/// Recursively-implemented depth-first find the first matching element. 
		/// Use for shallow drill-downs.
		xml_node first_element(const char* name) const;

		/// Recursively-implemented depth-first find the first matching element. 
		/// Use for shallow drill-downs.
		/// Enable wildcard matching.
		xml_node first_element_w(const char* name) const;

		/// Recursively-implemented depth-first find the first matching element 
		/// also having matching PCDATA.
		xml_node first_element_by_value(const char* name, const char* value) const;

		/// Recursively-implemented depth-first find the first matching element 
		/// also having matching PCDATA.
		/// Enable wildcard matching.
		xml_node first_element_by_value_w(const char* name, const char* value) const;

		/// Recursively-implemented depth-first find the first matching element 
		/// also having matching attribute.
		xml_node first_element_by_attribute(const char* name, const char* attr_name, const char* attr_value) const;

		/// Recursively-implemented depth-first find the first matching element 
		/// also having matching attribute.
		/// Enable wildcard matching.
		xml_node first_element_by_attribute_w(const char* name, const char* attr_name, const char* attr_value) const;

		/// Recursively-implemented depth-first find the first element 
		/// having matching attribute.
		xml_node first_element_by_attribute(const char* attr_name, const char* attr_value) const;

		/// Recursively-implemented depth-first find the first element 
		/// having matching attribute.
		/// Enable wildcard matching.
		xml_node first_element_by_attribute_w(const char* attr_name, const char* attr_value) const;

		/// Recursively-implemented depth-first find the first matching entity. 
		/// Use for shallow drill-downs.
		xml_node first_node(xml_node_type type) const;

		/// Compile the absolute node path from root as a text string.
		/// \param delimiter - Delimiter character to insert between element names.
		/// \return path string (e.g. with '/' as delimiter, '/document/.../this'.
		std::string path(char delimiter = '/') const;

		/// Search for a node by path.
		/// \param path - Path string; e.g. './foo/bar' (relative to node), '/foo/bar' (relative 
		/// to root), '../foo/bar' (pop relative position).
		/// \param delimiter - Delimiter character to use in tokenizing path.
		/// \return Matching node, or xml_node() if not found.
		xml_node first_element_by_path(const char* path, char delimiter = '/') const;

		/// Recursively traverse the tree.
		bool traverse(xml_tree_walker& walker) const;
	};

	/// Child node iterator.
	class xml_node_iterator: public std::iterator<std::bidirectional_iterator_tag, const xml_node>
	{
		friend class xml_node;

	private:
		xml_node _prev;
		xml_node _wrap;

		/// Initializing ctor
		explicit xml_node_iterator(const xml_node_struct* ref);
	public:
		/// Default ctor
		xml_node_iterator();

		/// Initializing ctor
		xml_node_iterator(const xml_node& node);

		/// Initializing ctor (for past-the-end)
		xml_node_iterator(const xml_node_struct* ref, const xml_node_struct* prev);

		bool operator==(const xml_node_iterator& rhs) const;
		bool operator!=(const xml_node_iterator& rhs) const;

		const xml_node& operator*() const;
		const xml_node* operator->() const;

		const xml_node_iterator& operator++();
		xml_node_iterator operator++(int);
		
		const xml_node_iterator& operator--();
		xml_node_iterator operator--(int);
	};

	/// Attribute iterator.
	class xml_attribute_iterator: public std::iterator<std::bidirectional_iterator_tag, const xml_attribute>
	{
		friend class xml_node;

	private:
		xml_attribute _prev;
		xml_attribute _wrap;

		/// Initializing ctor
		explicit xml_attribute_iterator(const xml_attribute_struct* ref);
	public:
		/// Default ctor
		xml_attribute_iterator();

		/// Initializing ctor
		xml_attribute_iterator(const xml_attribute& attr);

		/// Initializing ctor (for past-the-end)
		xml_attribute_iterator(const xml_attribute_struct* ref, const xml_attribute_struct* prev);

		bool operator==(const xml_attribute_iterator& rhs) const;
		bool operator!=(const xml_attribute_iterator& rhs) const;

		const xml_attribute& operator*() const;
		const xml_attribute* operator->() const;

		const xml_attribute_iterator& operator++();
		xml_attribute_iterator operator++(int);
		
		const xml_attribute_iterator& operator--();
		xml_attribute_iterator operator--(int);
	};

	/// Abstract tree walker class for xml_node::traverse().
	class xml_tree_walker
	{
	private:
		int _deep; ///< Current node depth.
	public:
		/// Default ctor
		xml_tree_walker();

		/// Virtual dtor
		virtual ~xml_tree_walker();

	public:
		/// Increment node depth.
		virtual void push();

		/// Decrement node depth
		virtual void pop();

		/// Access node depth
		virtual int depth() const;
	
	public:
		/// Callback when traverse on a node begins.
		/// \return returning false will abort the traversal.
		virtual bool begin(const xml_node&);

		/// Callback when traverse on a node ends.
		/// \return Returning false will abort the traversal.
		virtual bool end(const xml_node&);
	};

	/// Memory block (internal)
	struct xml_memory_block
	{
		xml_memory_block();

		xml_memory_block* next;
		size_t size;

		char data[memory_block_size];
	};

	/// Provides a high-level interface to the XML parser.
	class xml_parser
	{
	private:
		std::vector<char>	_buffer; ///< character buffer

		xml_memory_block	_memory; ///< Memory block
		
		xml_node_struct*	_xmldoc; ///< Pointer to current XML document tree root.
		unsigned int		_optmsk; ///< Parser options.
	
		xml_parser(const xml_parser&);
		const xml_parser& operator=(const xml_parser&);

	public:
		/// Constructor.
		/// \param optmsk - Options mask.
		xml_parser(unsigned int optmsk = parse_default);

		/// Parse constructor.
		/// \param xmlstr - readwrite string with xml data
		/// \param optmsk - Options mask.
		/// \see parse
		xml_parser(char* xmlstr, unsigned int optmsk = parse_default);

		/// Parse constructor.
		/// \param stream - stream with xml data
		/// \param optmsk - Options mask.
		/// \see parse
		xml_parser(std::istream& stream, unsigned int optmsk = parse_default);

		/// Dtor
		~xml_parser();

	public:
		/// Cast as xml_node (same as document).
		operator xml_node() const;

		/// Returns the root wrapped by an xml_node.
		xml_node document() const;

	public:
		/// Get parser options mask.
		unsigned int options() const;

		/// Set parser options mask.
		unsigned int options(unsigned int optmsk);

	public:
		/// Parse the given XML stream
		/// \param stream - stream with xml data
		/// \param optmsk - Options mask.
		void parse(std::istream& stream, unsigned int optmsk = parse_noset);

		/// Parse the given XML string in-situ.
		/// \param xmlstr - readwrite string with xml data
		/// \param optmsk - Options mask.
		/// \return last position or NULL
		/// \rem input string is zero-segmented
		char* parse(char* xmlstr, unsigned int optmsk = parse_noset);
	};

	/// Utility functions for xml
	
	/// Convert utf16 to utf8
	std::string utf8(const wchar_t* str);
	
	/// Convert utf8 to utf16
	std::wstring utf16(const char* str);
}

/// Inline implementation

namespace pugi
{
	namespace impl
	{
		int strcmpwild(const char*, const char*);
	}

	template <typename OutputIterator> void xml_node::all_elements_by_name(const char* name, OutputIterator it) const
	{
		if (empty()) return;
		
		for (xml_node node = first_child(); node; node = node.next_sibling())
		{
			if (!strcmp(name, node.name()))
			{
				*it = node;
				++it;
			}
			
			if (node.first_child()) node.all_elements_by_name(name, it);
		}
	}

	template <typename OutputIterator> void xml_node::all_elements_by_name_w(const char* name, OutputIterator it) const
	{
		if (empty()) return;
		
		for (xml_node node = first_child(); node; node = node.next_sibling())
		{
			if (!impl::strcmpwild(name, node.name()))
			{
				*it = node;
				++it;
			}
			
			if (node.first_child()) node.all_elements_by_name_w(name, it);
		}
	}
	
	template <typename Predicate> inline xml_attribute xml_node::find_attribute(Predicate pred) const
	{
		if (!empty())
			for (xml_attribute attrib = first_attribute(); attrib; attrib = attrib.next_attribute())
				if (pred(attrib))
					return attrib;
		
		return xml_attribute();
	}

	template <typename Predicate> inline xml_node xml_node::find_child(Predicate pred) const
	{
		if (!empty())
			for (xml_node node = first_child(); node; node = node.next_sibling())
				if (pred(node))
					return node;

		return xml_node();
	}

	template <typename Predicate> inline xml_node xml_node::find_element(Predicate pred) const
	{
		if (!empty())
			for (xml_node node = first_child(); node; node = node.next_sibling())
			{
				if (pred(node))
					return node;
				
				if (node.first_child())
				{
					xml_node found = node.find_element(pred);
					if (found) return found;
				}
			}

		return xml_node();
	}
}

#endif
