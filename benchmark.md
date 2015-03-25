---
layout: default
title: Benchmark
---

## Benchmarks

These charts summarize the performance and memory consumption comparison between pugixml and other XML parsers.

The time measured is the time to parse the document from memory; for DOM parsers, this involves the DOM tree construction, for SAX parsers this involves parsing the entire document with dummy handlers, for pull parsers this involves just parsing the entire document. In order to account for timing noise, the benchmark is run many times, and the minimum run time is recorded.

The memory consumption is the peak value of allocated memory during the parsing process; it is only displayed for DOM parsers, since SAX/pull parsers usually have more or less constant memory consumption.

*Note: pugixml_compact refers to new compact mode implementation from compact branch that is not released yet. That branch also reduces the memory requirements of baseline configuration - expect it to make it into next version!*

All tests were performed with Microsoft Visual Studio 2015, with all optimizations on and with all parsers operating in their default modes (i.e., no custom tuning has been performed). The timing results were recorded on a Intel Core i7 @ 2.67 GHz.

Benchmarks are performed on nine different test files; all of them are in 1-20 Mb range and display a different range of markup/content ratios - e.g. there's a COLLADA file that has little markup, there's a book file that has almost no markup, there are some files where there is a lot of markup and almost no content, etc. The test files include a 10 Mb [XMark](http://www.xml-benchmark.org) test file.

Due to a number of test files all results are normalized using pugixml results as a baseline - the ratio between time or memory for a particular parser to pugixml is displayed (so if it's 1 or more then the parser is slower or takes more memory than pugixml). Note that the horizontal axis is using logarithmic scale.

Benchmark results are visualized using [Google Charts](https://developers.google.com/chart/) and require JavaScript and SVG support to render.

<div id="chart_speed_x86" style="width: 600px; height: 500px; padding: 10px;">Parsing time (x86) relative to pugixml, lower is better</div>
<div id="chart_memory_x86" style="width: 600px; height: 350px; padding: 10px;">Parsing memory (x86) relative to pugixml, lower is better</div>
<div id="chart_speed_x64" style="width: 600px; height: 500px; padding: 10px;">Parsing time (x64) relative to pugixml, lower is better</div>
<div id="chart_memory_x64" style="width: 600px; height: 350px; padding: 10px;">Parsing memory (x64) relative to pugixml, lower is better</div>

<script type="text/javascript" src="benchmark-data.js"></script>

<script type="text/javascript" src="https://www.google.com/jsapi"></script>

<script type="text/javascript">
	bdata = {}
	bfiles = {}

	function hget(h, key, def) {
		if (h.hasOwnProperty(key)) {
			return h[key]
		} else {
			h[key] = def
			return h[key]
		}
	}

	function benchmark_data(category, platform, parser, file, value) {
		hget(hget(hget(bdata, category, {}), platform, {}), parser, {})[file] = value
		bfiles[file] = file
	}

	function drawChartRatio(bd, chartid, haxis) {
		var data = new google.visualization.DataTable()
		
		data.addColumn('string', 'parser')
	
		for (var file in bfiles) {
			data.addColumn('number', file)
		}

		for (var parser in bd) {
			var row = [parser]

			for (var file in bfiles) {
				var ratio = bd[parser][file] / bd['pugixml'][file]

				row.push(ratio)
			}

			data.addRow(row)
		}

		var chartdiv = document.getElementById(chartid)

		var options = {
			legend: 'none',
			orientation: 'vertical',
			title: chartdiv.innerHTML,
			lineWidth: 0,
			pointSize: 5,
			chartArea: {width: '65%', height: '90%'},
			hAxis: haxis
		};

		var chart = new google.visualization.LineChart(chartdiv)

		chart.draw(data, options)
	}

	google.load('visualization', '1.0', {'packages':['corechart']})

	google.setOnLoadCallback(function () {
		benchmark(benchmark_data)

		var hAxisSpeed = {
			logScale: true,
			minValue: 0.75,
			ticks: [1, 3, 9, 27, 81]
		}

		var hAxisMemory = {
			logScale: true,
			minValue: 0.25,
			ticks: [0.5, 1, 2, 4, 8]
		}

		drawChartRatio(bdata.speed.x86, 'chart_speed_x86', hAxisSpeed)
		drawChartRatio(bdata.memory.x86, 'chart_memory_x86', hAxisMemory)
		drawChartRatio(bdata.speed.x64, 'chart_speed_x64', hAxisSpeed)
		drawChartRatio(bdata.memory.x64, 'chart_memory_x64', hAxisMemory)
	})
</script>
