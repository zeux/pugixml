function benchmark(data) { 
// data/house.dae
// asmxml: 1.05 us/byte, 904 Mb/s, 7559 Kb, 2118 allocations
data("speed", "x86", "asmxml", "data/house.dae", 1.05494);
data("memory", "x86", "asmxml", "data/house.dae", 7.38267);
// cmarkup: 3.74 us/byte, 255 Mb/s, 15666 Kb, 17 allocations
data("speed", "x86", "cmarkup", "data/house.dae", 3.73726);
data("memory", "x86", "cmarkup", "data/house.dae", 15.2997);
// expat: 2.00 us/byte, 477 Mb/s, 8203 Kb, 164 allocations
data("speed", "x86", "expat", "data/house.dae", 1.99812);
// irrxml: 7.82 us/byte, 122 Mb/s, 6670 Kb, 170531 allocations
data("speed", "x86", "irrxml", "data/house.dae", 7.82061);
// ixmlparser_dom: 7.33 us/byte, 130 Mb/s, 8193 Kb, 14 allocations
data("speed", "x86", "ixmlparser_dom", "data/house.dae", 7.3291);
data("memory", "x86", "ixmlparser_dom", "data/house.dae", 8.00116);
// ixmlparser_pull: 6.33 us/byte, 151 Mb/s, 1024 Kb, 2 allocations
data("speed", "x86", "ixmlparser_pull", "data/house.dae", 6.3292);
// libxml_dom: 2.50 us/byte, 381 Mb/s, 16446 Kb, 57098 allocations
data("speed", "x86", "libxml_dom", "data/house.dae", 2.5007);
data("memory", "x86", "libxml_dom", "data/house.dae", 16.0611);
// libxml_sax: 1.50 us/byte, 636 Mb/s, 8205 Kb, 6964 allocations
data("speed", "x86", "libxml_sax", "data/house.dae", 1.50008);
// msxml_dom: 7.06 us/byte, 135 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "msxml_dom", "data/house.dae", 7.06304);
data("memory", "x86", "msxml_dom", "data/house.dae", 12.5118);
// msxml_sax: 9.39 us/byte, 102 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "msxml_sax", "data/house.dae", 9.3922);
// pugixml: 0.56 us/byte, 1714 Mb/s, 6651 Kb, 13 allocations
data("speed", "x86", "pugixml", "data/house.dae", 0.55643);
data("memory", "x86", "pugixml", "data/house.dae", 6.4953);
// pugixml_compact: 0.60 us/byte, 1586 Mb/s, 6466 Kb, 13 allocations
data("speed", "x86", "pugixml_compact", "data/house.dae", 0.60115);
data("memory", "x86", "pugixml_compact", "data/house.dae", 6.31496);
// pugxml: 3.72 us/byte, 257 Mb/s, 7005 Kb, 37428 allocations
data("speed", "x86", "pugxml", "data/house.dae", 3.71753);
data("memory", "x86", "pugxml", "data/house.dae", 6.84106);
// qtxml_dom: 22.87 us/byte, 42 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "qtxml_dom", "data/house.dae", 22.8749);
data("memory", "x86", "qtxml_dom", "data/house.dae", 19.831);
// qtxml_sax: 18.57 us/byte, 51 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "qtxml_sax", "data/house.dae", 18.5741);
// rapidxml: 0.71 us/byte, 1335 Mb/s, 6778 Kb, 9 allocations
data("speed", "x86", "rapidxml", "data/house.dae", 0.714515);
data("memory", "x86", "rapidxml", "data/house.dae", 6.61939);
// tinyxml: 40.27 us/byte, 24 Mb/s, 10188 Kb, 66621 allocations
data("speed", "x86", "tinyxml", "data/house.dae", 40.271);
data("memory", "x86", "tinyxml", "data/house.dae", 9.94964);
// tinyxml2: 2.38 us/byte, 401 Mb/s, 6879 Kb, 162 allocations
data("speed", "x86", "tinyxml2", "data/house.dae", 2.37774);
data("memory", "x86", "tinyxml2", "data/house.dae", 6.71816);
// vtdxml: 4.38 us/byte, 218 Mb/s, 6539 Kb, 24 allocations
data("speed", "x86", "vtdxml", "data/house.dae", 4.38);
data("memory", "x86", "vtdxml", "data/house.dae", 6.38584);
// xerces_dom: 3.43 us/byte, 278 Mb/s, 15260 Kb, 5520 allocations
data("speed", "x86", "xerces_dom", "data/house.dae", 3.43135);
data("memory", "x86", "xerces_dom", "data/house.dae", 14.9027);
// xerces_sax: 2.61 us/byte, 365 Mb/s, 847 Kb, 739 allocations
data("speed", "x86", "xerces_sax", "data/house.dae", 2.61334);
// xmllite: 2.26 us/byte, 423 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "xmllite", "data/house.dae", 2.25544);
// xmlparser: 13.62 us/byte, 70 Mb/s, 6788 Kb, 48447 allocations
data("speed", "x86", "xmlparser", "data/house.dae", 13.6178);
data("memory", "x86", "xmlparser", "data/house.dae", 6.62947);

// data/English.sml
// asmxml: 1.15 us/byte, 831 Mb/s, 2703 Kb, 39 allocations
data("speed", "x86", "asmxml", "data/English.sml", 1.14806);
data("memory", "x86", "asmxml", "data/English.sml", 2.64017);
// cmarkup: 4.19 us/byte, 228 Mb/s, 3430 Kb, 7 allocations
data("speed", "x86", "cmarkup", "data/English.sml", 4.18978);
data("memory", "x86", "cmarkup", "data/English.sml", 3.34965);
// expat: 2.12 us/byte, 450 Mb/s, 2053 Kb, 22 allocations
data("speed", "x86", "expat", "data/English.sml", 2.11833);
// irrxml: 9.81 us/byte, 97 Mb/s, 1379 Kb, 64680 allocations
data("speed", "x86", "irrxml", "data/English.sml", 9.81227);
// ixmlparser_dom: 9.08 us/byte, 105 Mb/s, 3197 Kb, 23 allocations
data("speed", "x86", "ixmlparser_dom", "data/English.sml", 9.07521);
data("memory", "x86", "ixmlparser_dom", "data/English.sml", 3.12243);
// ixmlparser_pull: 7.29 us/byte, 131 Mb/s, 1024 Kb, 2 allocations
data("speed", "x86", "ixmlparser_pull", "data/English.sml", 7.28994);
// libxml_dom: 6.41 us/byte, 149 Mb/s, 4964 Kb, 37253 allocations
data("speed", "x86", "libxml_dom", "data/English.sml", 6.41223);
data("memory", "x86", "libxml_dom", "data/English.sml", 4.84845);
// libxml_sax: 3.22 us/byte, 296 Mb/s, 2059 Kb, 20 allocations
data("speed", "x86", "libxml_sax", "data/English.sml", 3.21799);
// msxml_dom: 11.43 us/byte, 83 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "msxml_dom", "data/English.sml", 11.4334);
data("memory", "x86", "msxml_dom", "data/English.sml", 2.78545);
// msxml_sax: 13.03 us/byte, 73 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "msxml_sax", "data/English.sml", 13.0296);
// pugixml: 0.77 us/byte, 1237 Mb/s, 1949 Kb, 19 allocations
data("speed", "x86", "pugixml", "data/English.sml", 0.770856);
data("memory", "x86", "pugixml", "data/English.sml", 1.90366);
// pugixml_compact: 1.22 us/byte, 783 Mb/s, 1628 Kb, 16 allocations
data("speed", "x86", "pugixml_compact", "data/English.sml", 1.21748);
data("memory", "x86", "pugixml_compact", "data/English.sml", 1.59004);
// pugxml: 9.42 us/byte, 101 Mb/s, 2650 Kb, 47830 allocations
data("speed", "x86", "pugxml", "data/English.sml", 9.41984);
data("memory", "x86", "pugxml", "data/English.sml", 2.58832);
// qtxml_dom: 29.54 us/byte, 32 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "qtxml_dom", "data/English.sml", 29.544);
data("memory", "x86", "qtxml_dom", "data/English.sml", 5.50416);
// qtxml_sax: 19.48 us/byte, 49 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "qtxml_sax", "data/English.sml", 19.4842);
// rapidxml: 0.99 us/byte, 961 Mb/s, 2203 Kb, 14 allocations
data("speed", "x86", "rapidxml", "data/English.sml", 0.992795);
data("memory", "x86", "rapidxml", "data/English.sml", 2.1523);
// tinyxml: 50.65 us/byte, 19 Mb/s, 3522 Kb, 100500 allocations
data("speed", "x86", "tinyxml", "data/English.sml", 50.653);
data("memory", "x86", "tinyxml", "data/English.sml", 3.43995);
// tinyxml2: 4.11 us/byte, 232 Mb/s, 2260 Kb, 232 allocations
data("speed", "x86", "tinyxml2", "data/English.sml", 4.1091);
data("memory", "x86", "tinyxml2", "data/English.sml", 2.20755);
// vtdxml: 4.66 us/byte, 204 Mb/s, 1676 Kb, 28 allocations
data("speed", "x86", "vtdxml", "data/English.sml", 4.6644);
data("memory", "x86", "vtdxml", "data/English.sml", 1.63745);
// xerces_dom: 5.49 us/byte, 174 Mb/s, 4646 Kb, 3276 allocations
data("speed", "x86", "xerces_dom", "data/English.sml", 5.49432);
data("memory", "x86", "xerces_dom", "data/English.sml", 4.53774);
// xerces_sax: 3.78 us/byte, 252 Mb/s, 286 Kb, 213 allocations
data("speed", "x86", "xerces_sax", "data/English.sml", 3.7806);
// xmllite: 3.85 us/byte, 247 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "xmllite", "data/English.sml", 3.85336);
// xmlparser: 19.49 us/byte, 49 Mb/s, 2343 Kb, 60082 allocations
data("speed", "x86", "xmlparser", "data/English.sml", 19.4942);
data("memory", "x86", "xmlparser", "data/English.sml", 2.28828);

// data/Russian.sml
// asmxml: 1.09 us/byte, 874 Mb/s, 3930 Kb, 39 allocations
data("speed", "x86", "asmxml", "data/Russian.sml", 1.0911);
data("memory", "x86", "asmxml", "data/Russian.sml", 3.83864);
// cmarkup: 3.92 us/byte, 243 Mb/s, 6385 Kb, 7 allocations
data("speed", "x86", "cmarkup", "data/Russian.sml", 3.92127);
data("memory", "x86", "cmarkup", "data/Russian.sml", 6.23626);
// expat: 3.78 us/byte, 252 Mb/s, 4101 Kb, 22 allocations
data("speed", "x86", "expat", "data/Russian.sml", 3.78213);
// irrxml: 7.64 us/byte, 125 Mb/s, 2567 Kb, 61129 allocations
data("speed", "x86", "irrxml", "data/Russian.sml", 7.64176);
// ixmlparser_dom: 5.57 us/byte, 171 Mb/s, 5220 Kb, 25 allocations
data("speed", "x86", "ixmlparser_dom", "data/Russian.sml", 5.57173);
data("memory", "x86", "ixmlparser_dom", "data/Russian.sml", 5.09803);
// ixmlparser_pull: 4.19 us/byte, 227 Mb/s, 1024 Kb, 2 allocations
data("speed", "x86", "ixmlparser_pull", "data/Russian.sml", 4.19384);
// libxml_dom: 14.42 us/byte, 66 Mb/s, 9402 Kb, 39784 allocations
data("speed", "x86", "libxml_dom", "data/Russian.sml", 14.4163);
data("memory", "x86", "libxml_dom", "data/Russian.sml", 9.18179);
// libxml_sax: 12.18 us/byte, 78 Mb/s, 4108 Kb, 20 allocations
data("speed", "x86", "libxml_sax", "data/Russian.sml", 12.178);
// msxml_dom: 8.17 us/byte, 117 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "msxml_dom", "data/Russian.sml", 8.17234);
data("memory", "x86", "msxml_dom", "data/Russian.sml", 5.16508);
// msxml_sax: 11.68 us/byte, 82 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "msxml_sax", "data/Russian.sml", 11.6772);
// pugixml: 0.64 us/byte, 1493 Mb/s, 3099 Kb, 18 allocations
data("speed", "x86", "pugixml", "data/Russian.sml", 0.638829);
data("memory", "x86", "pugixml", "data/Russian.sml", 3.02695);
// pugixml_compact: 0.86 us/byte, 1104 Mb/s, 2794 Kb, 15 allocations
data("speed", "x86", "pugixml_compact", "data/Russian.sml", 0.863491);
data("memory", "x86", "pugixml_compact", "data/Russian.sml", 2.72905);
// pugxml: 5.24 us/byte, 182 Mb/s, 3743 Kb, 44471 allocations
data("speed", "x86", "pugxml", "data/Russian.sml", 5.24095);
data("memory", "x86", "pugxml", "data/Russian.sml", 3.65545);
// qtxml_dom: 26.71 us/byte, 36 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "qtxml_dom", "data/Russian.sml", 26.7139);
data("memory", "x86", "qtxml_dom", "data/Russian.sml", 5.63391);
// qtxml_sax: 11.73 us/byte, 81 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "qtxml_sax", "data/Russian.sml", 11.7337);
// rapidxml: 0.87 us/byte, 1101 Mb/s, 3322 Kb, 13 allocations
data("speed", "x86", "rapidxml", "data/Russian.sml", 0.866434);
data("memory", "x86", "rapidxml", "data/Russian.sml", 3.24442);
// tinyxml: 30.37 us/byte, 31 Mb/s, 5140 Kb, 95136 allocations
data("speed", "x86", "tinyxml", "data/Russian.sml", 30.3667);
data("memory", "x86", "tinyxml", "data/Russian.sml", 5.01982);
// tinyxml2: 2.86 us/byte, 334 Mb/s, 3382 Kb, 216 allocations
data("speed", "x86", "tinyxml2", "data/Russian.sml", 2.8565);
data("memory", "x86", "tinyxml2", "data/Russian.sml", 3.30355);
// vtdxml: 5.94 us/byte, 161 Mb/s, 2859 Kb, 28 allocations
data("speed", "x86", "vtdxml", "data/Russian.sml", 5.93636);
data("memory", "x86", "vtdxml", "data/Russian.sml", 2.79208);
// xerces_dom: 7.55 us/byte, 126 Mb/s, 4879 Kb, 3238 allocations
data("speed", "x86", "xerces_dom", "data/Russian.sml", 7.55395);
data("memory", "x86", "xerces_dom", "data/Russian.sml", 4.76544);
// xerces_sax: 6.60 us/byte, 144 Mb/s, 283 Kb, 212 allocations
data("speed", "x86", "xerces_sax", "data/Russian.sml", 6.60413);
// xmllite: 5.93 us/byte, 161 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "xmllite", "data/Russian.sml", 5.929);
// xmlparser: 12.60 us/byte, 76 Mb/s, 3457 Kb, 55836 allocations
data("speed", "x86", "xmlparser", "data/Russian.sml", 12.6038);
data("memory", "x86", "xmlparser", "data/Russian.sml", 3.37626);

// data/blog.xml
// asmxml: 1.36 us/byte, 703 Mb/s, 3495 Kb, 444 allocations
data("speed", "x86", "asmxml", "data/blog.xml", 1.35746);
data("memory", "x86", "asmxml", "data/blog.xml", 3.41327);
// cmarkup: 4.80 us/byte, 199 Mb/s, 3260 Kb, 9 allocations
data("speed", "x86", "cmarkup", "data/blog.xml", 4.79596);
data("memory", "x86", "cmarkup", "data/blog.xml", 3.18419);
// expat: 5.14 us/byte, 185 Mb/s, 2056 Kb, 137 allocations
data("speed", "x86", "expat", "data/blog.xml", 5.14205);
// irrxml: 19.39 us/byte, 49 Mb/s, 1323 Kb, 171511 allocations
data("speed", "x86", "irrxml", "data/blog.xml", 19.3944);
// ixmlparser_dom: 9.48 us/byte, 101 Mb/s, 3080 Kb, 16 allocations
data("speed", "x86", "ixmlparser_dom", "data/blog.xml", 9.47783);
data("memory", "x86", "ixmlparser_dom", "data/blog.xml", 3.00803);
// ixmlparser_pull: 7.29 us/byte, 131 Mb/s, 1024 Kb, 2 allocations
data("speed", "x86", "ixmlparser_pull", "data/blog.xml", 7.28623);
// libxml_dom: 10.20 us/byte, 94 Mb/s, 6262 Kb, 79871 allocations
data("speed", "x86", "libxml_dom", "data/blog.xml", 10.1953);
data("memory", "x86", "libxml_dom", "data/blog.xml", 6.11604);
// libxml_sax: 3.95 us/byte, 241 Mb/s, 2068 Kb, 5238 allocations
data("speed", "x86", "libxml_sax", "data/blog.xml", 3.95027);
// msxml_dom: 19.03 us/byte, 50 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "msxml_dom", "data/blog.xml", 19.0281);
data("memory", "x86", "msxml_dom", "data/blog.xml", 2.66241);
// msxml_sax: 13.17 us/byte, 72 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "msxml_sax", "data/blog.xml", 13.1673);
// pugixml: 1.17 us/byte, 816 Mb/s, 2041 Kb, 24 allocations
data("speed", "x86", "pugixml", "data/blog.xml", 1.16903);
data("memory", "x86", "pugixml", "data/blog.xml", 1.99341);
// pugixml_compact: 1.59 us/byte, 602 Mb/s, 1639 Kb, 18 allocations
data("speed", "x86", "pugixml_compact", "data/blog.xml", 1.5853);
data("memory", "x86", "pugixml_compact", "data/blog.xml", 1.60143);
// pugxml: 8.59 us/byte, 111 Mb/s, 2823 Kb, 64805 allocations
data("speed", "x86", "pugxml", "data/blog.xml", 8.59113);
data("memory", "x86", "pugxml", "data/blog.xml", 2.75732);
// qtxml_dom: 43.64 us/byte, 22 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "qtxml_dom", "data/blog.xml", 43.6395);
data("memory", "x86", "qtxml_dom", "data/blog.xml", 6.84798);
// qtxml_sax: 30.10 us/byte, 32 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "qtxml_sax", "data/blog.xml", 30.1037);
// rapidxml: 1.32 us/byte, 720 Mb/s, 2391 Kb, 18 allocations
data("speed", "x86", "rapidxml", "data/blog.xml", 1.32381);
data("memory", "x86", "rapidxml", "data/blog.xml", 2.33544);
// tinyxml: 50.29 us/byte, 19 Mb/s, 3954 Kb, 138821 allocations
data("speed", "x86", "tinyxml", "data/blog.xml", 50.2948);
data("memory", "x86", "tinyxml", "data/blog.xml", 3.86146);
// tinyxml2: 5.53 us/byte, 173 Mb/s, 2490 Kb, 309 allocations
data("speed", "x86", "tinyxml2", "data/blog.xml", 5.5282);
data("memory", "x86", "tinyxml2", "data/blog.xml", 2.4325);
// vtdxml: 5.23 us/byte, 182 Mb/s, 1594 Kb, 28 allocations
data("speed", "x86", "vtdxml", "data/blog.xml", 5.22665);
data("memory", "x86", "vtdxml", "data/blog.xml", 1.55684);
// xerces_dom: 11.93 us/byte, 80 Mb/s, 6384 Kb, 3588 allocations
data("speed", "x86", "xerces_dom", "data/blog.xml", 11.9296);
data("memory", "x86", "xerces_dom", "data/blog.xml", 6.2345);
// xerces_sax: 8.05 us/byte, 118 Mb/s, 236 Kb, 510 allocations
data("speed", "x86", "xerces_sax", "data/blog.xml", 8.04924);
// xmllite: 4.05 us/byte, 235 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "xmllite", "data/blog.xml", 4.05058);
// xmlparser: 21.92 us/byte, 43 Mb/s, 2460 Kb, 91388 allocations
data("speed", "x86", "xmlparser", "data/blog.xml", 21.9247);
data("memory", "x86", "xmlparser", "data/blog.xml", 2.40239);

// data/cathedral.xml
// asmxml: 1.20 us/byte, 791 Mb/s, 2687 Kb, 56 allocations
data("speed", "x86", "asmxml", "data/cathedral.xml", 1.20498);
data("memory", "x86", "asmxml", "data/cathedral.xml", 2.62406);
// cmarkup: 6.86 us/byte, 139 Mb/s, 2946 Kb, 8 allocations
data("speed", "x86", "cmarkup", "data/cathedral.xml", 6.8562);
data("memory", "x86", "cmarkup", "data/cathedral.xml", 2.87747);
// expat: 17.39 us/byte, 55 Mb/s, 1028 Kb, 23 allocations
data("speed", "x86", "expat", "data/cathedral.xml", 17.3882);
// irrxml: 70.96 us/byte, 13 Mb/s, 906 Kb, 657154 allocations
data("speed", "x86", "irrxml", "data/cathedral.xml", 70.9604);
// ixmlparser_dom: 10.57 us/byte, 90 Mb/s, 4146 Kb, 22 allocations
data("speed", "x86", "ixmlparser_dom", "data/cathedral.xml", 10.5711);
data("memory", "x86", "ixmlparser_dom", "data/cathedral.xml", 4.04919);
// ixmlparser_pull: 5.70 us/byte, 167 Mb/s, 1024 Kb, 2 allocations
data("speed", "x86", "ixmlparser_pull", "data/cathedral.xml", 5.6996);
// libxml_dom: 31.88 us/byte, 30 Mb/s, 9591 Kb, 225750 allocations
data("speed", "x86", "libxml_dom", "data/cathedral.xml", 31.8766);
data("memory", "x86", "libxml_dom", "data/cathedral.xml", 9.36716);
// libxml_sax: 11.46 us/byte, 83 Mb/s, 1035 Kb, 30115 allocations
data("speed", "x86", "libxml_sax", "data/cathedral.xml", 11.4619);
// msxml_dom: 37.73 us/byte, 25 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "msxml_dom", "data/cathedral.xml", 37.7264);
data("memory", "x86", "msxml_dom", "data/cathedral.xml", 1.84982);
// msxml_sax: 14.34 us/byte, 67 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "msxml_sax", "data/cathedral.xml", 14.3391);
// pugixml: 1.36 us/byte, 699 Mb/s, 2125 Kb, 39 allocations
data("speed", "x86", "pugixml", "data/cathedral.xml", 1.36396);
data("memory", "x86", "pugixml", "data/cathedral.xml", 2.07588);
// pugixml_compact: 2.35 us/byte, 407 Mb/s, 1516 Kb, 23 allocations
data("speed", "x86", "pugixml_compact", "data/cathedral.xml", 2.34546);
data("memory", "x86", "pugixml_compact", "data/cathedral.xml", 1.48102);
// pugxml: 18.26 us/byte, 52 Mb/s, 3452 Kb, 136702 allocations
data("speed", "x86", "pugxml", "data/cathedral.xml", 18.2609);
data("memory", "x86", "pugxml", "data/cathedral.xml", 3.37138);
// qtxml_dom: 87.27 us/byte, 11 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "qtxml_dom", "data/cathedral.xml", 87.269);
data("memory", "x86", "qtxml_dom", "data/cathedral.xml", 10.9075);
// qtxml_sax: 57.86 us/byte, 16 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "qtxml_sax", "data/cathedral.xml", 57.8558);
// rapidxml: 1.64 us/byte, 582 Mb/s, 2634 Kb, 28 allocations
data("speed", "x86", "rapidxml", "data/cathedral.xml", 1.63721);
data("memory", "x86", "rapidxml", "data/cathedral.xml", 2.57301);
// tinyxml: 59.91 us/byte, 16 Mb/s, 5290 Kb, 240788 allocations
data("speed", "x86", "tinyxml", "data/cathedral.xml", 59.911);
data("memory", "x86", "tinyxml", "data/cathedral.xml", 5.16678);
// tinyxml2: 14.80 us/byte, 64 Mb/s, 3000 Kb, 537 allocations
data("speed", "x86", "tinyxml2", "data/cathedral.xml", 14.8003);
data("memory", "x86", "tinyxml2", "data/cathedral.xml", 2.92971);
// vtdxml: 6.64 us/byte, 144 Mb/s, 1797 Kb, 58 allocations
data("speed", "x86", "vtdxml", "data/cathedral.xml", 6.6364);
data("memory", "x86", "vtdxml", "data/cathedral.xml", 1.75542);
// xerces_dom: 31.75 us/byte, 30 Mb/s, 9429 Kb, 249 allocations
data("speed", "x86", "xerces_dom", "data/cathedral.xml", 31.7544);
data("memory", "x86", "xerces_dom", "data/cathedral.xml", 9.20844);
// xerces_sax: 20.33 us/byte, 47 Mb/s, 244 Kb, 221 allocations
data("speed", "x86", "xerces_sax", "data/cathedral.xml", 20.335);
// xmllite: 6.93 us/byte, 138 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "xmllite", "data/cathedral.xml", 6.93315);
// xmlparser: 26.25 us/byte, 36 Mb/s, 3069 Kb, 170699 allocations
data("speed", "x86", "xmlparser", "data/cathedral.xml", 26.2535);
data("memory", "x86", "xmlparser", "data/cathedral.xml", 2.99764);

// data/employees-big.xml
// asmxml: 1.49 us/byte, 641 Mb/s, 19181 Kb, 285 allocations
data("speed", "x86", "asmxml", "data/employees-big.xml", 1.48853);
data("memory", "x86", "asmxml", "data/employees-big.xml", 18.7317);
// cmarkup: 6.59 us/byte, 145 Mb/s, 28670 Kb, 10 allocations
data("speed", "x86", "cmarkup", "data/employees-big.xml", 6.59347);
data("memory", "x86", "cmarkup", "data/employees-big.xml", 27.9988);
// expat: 12.04 us/byte, 79 Mb/s, 16389 Kb, 23 allocations
data("speed", "x86", "expat", "data/employees-big.xml", 12.0415);
// irrxml: 40.12 us/byte, 24 Mb/s, 10239 Kb, 3963353 allocations
data("speed", "x86", "irrxml", "data/employees-big.xml", 40.1225);
// ixmlparser_dom: 11.08 us/byte, 86 Mb/s, 24976 Kb, 48 allocations
data("speed", "x86", "ixmlparser_dom", "data/employees-big.xml", 11.0825);
data("memory", "x86", "ixmlparser_dom", "data/employees-big.xml", 24.3911);
// ixmlparser_pull: 6.12 us/byte, 156 Mb/s, 1024 Kb, 2 allocations
data("speed", "x86", "ixmlparser_pull", "data/employees-big.xml", 6.11578);
// libxml_dom: 23.79 us/byte, 40 Mb/s, 88863 Kb, 1777509 allocations
data("speed", "x86", "libxml_dom", "data/employees-big.xml", 23.7916);
data("memory", "x86", "libxml_dom", "data/employees-big.xml", 86.781);
// libxml_sax: 7.28 us/byte, 131 Mb/s, 16399 Kb, 84088 allocations
data("speed", "x86", "libxml_sax", "data/employees-big.xml", 7.28231);
// msxml_dom: 31.96 us/byte, 30 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "msxml_dom", "data/employees-big.xml", 31.962);
data("memory", "x86", "msxml_dom", "data/employees-big.xml", 20.7993);
// msxml_sax: 13.73 us/byte, 69 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "msxml_sax", "data/employees-big.xml", 13.729);
// pugixml: 1.54 us/byte, 619 Mb/s, 25449 Kb, 475 allocations
data("speed", "x86", "pugixml", "data/employees-big.xml", 1.54028);
data("memory", "x86", "pugixml", "data/employees-big.xml", 24.8532);
// pugixml_compact: 2.54 us/byte, 375 Mb/s, 16587 Kb, 202 allocations
data("speed", "x86", "pugixml_compact", "data/employees-big.xml", 2.54028);
data("memory", "x86", "pugixml_compact", "data/employees-big.xml", 16.199);
// pugxml: 22.43 us/byte, 43 Mb/s, 43171 Kb, 1369156 allocations
data("speed", "x86", "pugxml", "data/employees-big.xml", 22.4336);
data("memory", "x86", "pugxml", "data/employees-big.xml", 42.1592);
// qtxml_dom: 65.40 us/byte, 15 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "qtxml_dom", "data/employees-big.xml", 65.3968);
data("memory", "x86", "qtxml_dom", "data/employees-big.xml", 85.5823);
// qtxml_sax: 38.38 us/byte, 25 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "qtxml_sax", "data/employees-big.xml", 38.3792);
// rapidxml: 1.75 us/byte, 544 Mb/s, 32773 Kb, 353 allocations
data("speed", "x86", "rapidxml", "data/employees-big.xml", 1.75335);
data("memory", "x86", "rapidxml", "data/employees-big.xml", 32.0052);
// tinyxml: 62.85 us/byte, 15 Mb/s, 54796 Kb, 2450061 allocations
data("speed", "x86", "tinyxml", "data/employees-big.xml", 62.8452);
data("memory", "x86", "tinyxml", "data/employees-big.xml", 53.5118);
// tinyxml2: 15.58 us/byte, 61 Mb/s, 34493 Kb, 6110 allocations
data("speed", "x86", "tinyxml2", "data/employees-big.xml", 15.5797);
data("memory", "x86", "tinyxml2", "data/employees-big.xml", 33.6852);
// vtdxml: 5.39 us/byte, 177 Mb/s, 16990 Kb, 603 allocations
data("speed", "x86", "vtdxml", "data/employees-big.xml", 5.38547);
data("memory", "x86", "vtdxml", "data/employees-big.xml", 16.5924);
// xerces_dom: 22.39 us/byte, 43 Mb/s, 80257 Kb, 399 allocations
data("speed", "x86", "xerces_dom", "data/employees-big.xml", 22.3919);
data("memory", "x86", "xerces_dom", "data/employees-big.xml", 78.376);
// xerces_sax: 14.15 us/byte, 67 Mb/s, 551 Kb, 233 allocations
data("speed", "x86", "xerces_sax", "data/employees-big.xml", 14.1453);
// xmllite: 7.19 us/byte, 133 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "xmllite", "data/employees-big.xml", 7.18768);
// xmlparser: 30.17 us/byte, 32 Mb/s, 33535 Kb, 1898880 allocations
data("speed", "x86", "xmlparser", "data/employees-big.xml", 30.1717);
data("memory", "x86", "xmlparser", "data/employees-big.xml", 32.7492);

// data/terrover.xml
// asmxml: 1.18 us/byte, 805 Mb/s, 22824 Kb, 40740 allocations
data("speed", "x86", "asmxml", "data/terrover.xml", 1.18438);
data("memory", "x86", "asmxml", "data/terrover.xml", 22.2896);
// cmarkup: 5.68 us/byte, 168 Mb/s, 38444 Kb, 12 allocations
data("speed", "x86", "cmarkup", "data/terrover.xml", 5.67974);
data("memory", "x86", "cmarkup", "data/terrover.xml", 37.5436);
// expat: 14.87 us/byte, 64 Mb/s, 16397 Kb, 132 allocations
data("speed", "x86", "expat", "data/terrover.xml", 14.8652);
// irrxml: 34.79 us/byte, 27 Mb/s, 16164 Kb, 4961420 allocations
data("speed", "x86", "irrxml", "data/terrover.xml", 34.793);
// ixmlparser_dom: 8.80 us/byte, 108 Mb/s, 26637 Kb, 40 allocations
data("speed", "x86", "ixmlparser_dom", "data/terrover.xml", 8.80217);
data("memory", "x86", "ixmlparser_dom", "data/terrover.xml", 26.0127);
// ixmlparser_pull: 6.19 us/byte, 154 Mb/s, 1024 Kb, 2 allocations
data("speed", "x86", "ixmlparser_pull", "data/terrover.xml", 6.19329);
// libxml_dom: 15.57 us/byte, 61 Mb/s, 89817 Kb, 1652648 allocations
data("speed", "x86", "libxml_dom", "data/terrover.xml", 15.5706);
data("memory", "x86", "libxml_dom", "data/terrover.xml", 87.7123);
// libxml_sax: 6.54 us/byte, 146 Mb/s, 16400 Kb, 230832 allocations
data("speed", "x86", "libxml_sax", "data/terrover.xml", 6.54386);
// msxml_dom: 24.08 us/byte, 40 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "msxml_dom", "data/terrover.xml", 24.0824);
data("memory", "x86", "msxml_dom", "data/terrover.xml", 32.7586);
// msxml_sax: 11.08 us/byte, 86 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "msxml_sax", "data/terrover.xml", 11.0793);
// pugixml: 1.01 us/byte, 948 Mb/s, 25776 Kb, 301 allocations
data("speed", "x86", "pugixml", "data/terrover.xml", 1.00624);
data("memory", "x86", "pugixml", "data/terrover.xml", 25.1727);
// pugixml_compact: 1.50 us/byte, 638 Mb/s, 20927 Kb, 156 allocations
data("speed", "x86", "pugixml_compact", "data/terrover.xml", 1.49517);
data("memory", "x86", "pugixml_compact", "data/terrover.xml", 20.437);
// pugxml: 9.77 us/byte, 98 Mb/s, 35534 Kb, 1031012 allocations
data("speed", "x86", "pugxml", "data/terrover.xml", 9.7682);
data("memory", "x86", "pugxml", "data/terrover.xml", 34.7016);
// qtxml_dom: 70.84 us/byte, 13 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "qtxml_dom", "data/terrover.xml", 70.8351);
data("memory", "x86", "qtxml_dom", "data/terrover.xml", 114.899);
// qtxml_sax: 54.25 us/byte, 18 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "qtxml_sax", "data/terrover.xml", 54.2507);
// rapidxml: 1.19 us/byte, 803 Mb/s, 30041 Kb, 218 allocations
data("speed", "x86", "rapidxml", "data/terrover.xml", 1.18805);
data("memory", "x86", "rapidxml", "data/terrover.xml", 29.3376);
// tinyxml: 48.03 us/byte, 20 Mb/s, 52939 Kb, 2131643 allocations
data("speed", "x86", "tinyxml", "data/terrover.xml", 48.0267);
data("memory", "x86", "tinyxml", "data/terrover.xml", 51.699);
// tinyxml2: 8.42 us/byte, 113 Mb/s, 32572 Kb, 4143 allocations
data("speed", "x86", "tinyxml2", "data/terrover.xml", 8.41985);
data("memory", "x86", "tinyxml2", "data/terrover.xml", 31.8095);
// vtdxml: 6.44 us/byte, 148 Mb/s, 21341 Kb, 55 allocations
data("speed", "x86", "vtdxml", "data/terrover.xml", 6.43812);
data("memory", "x86", "vtdxml", "data/terrover.xml", 20.8409);
// xerces_dom: 24.37 us/byte, 39 Mb/s, 83901 Kb, 7388 allocations
data("speed", "x86", "xerces_dom", "data/terrover.xml", 24.3731);
data("memory", "x86", "xerces_dom", "data/terrover.xml", 81.9352);
// xerces_sax: 18.23 us/byte, 52 Mb/s, 236 Kb, 509 allocations
data("speed", "x86", "xerces_sax", "data/terrover.xml", 18.2275);
// xmllite: 4.34 us/byte, 220 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "xmllite", "data/terrover.xml", 4.33776);
// xmlparser: 18.12 us/byte, 53 Mb/s, 32537 Kb, 1234038 allocations
data("speed", "x86", "xmlparser", "data/terrover.xml", 18.119);
data("memory", "x86", "xmlparser", "data/terrover.xml", 31.7753);

// data/vod.xml
// asmxml: 1.61 us/byte, 594 Mb/s, 5589 Kb, 4723 allocations
data("speed", "x86", "asmxml", "data/vod.xml", 1.60645);
data("memory", "x86", "asmxml", "data/vod.xml", 5.45857);
// cmarkup: 7.55 us/byte, 126 Mb/s, 8216 Kb, 12 allocations
data("speed", "x86", "cmarkup", "data/vod.xml", 7.55323);
data("memory", "x86", "cmarkup", "data/vod.xml", 8.02371);
// expat: 12.59 us/byte, 76 Mb/s, 4102 Kb, 71 allocations
data("speed", "x86", "expat", "data/vod.xml", 12.587);
// irrxml: 24.79 us/byte, 38 Mb/s, 2134 Kb, 461914 allocations
data("speed", "x86", "irrxml", "data/vod.xml", 24.7884);
// ixmlparser_dom: 13.25 us/byte, 72 Mb/s, 8204 Kb, 22 allocations
data("speed", "x86", "ixmlparser_dom", "data/vod.xml", 13.2491);
data("memory", "x86", "ixmlparser_dom", "data/vod.xml", 8.0126);
// ixmlparser_pull: 7.00 us/byte, 136 Mb/s, 1024 Kb, 2 allocations
data("speed", "x86", "ixmlparser_pull", "data/vod.xml", 7.00097);
// libxml_dom: 28.93 us/byte, 33 Mb/s, 22147 Kb, 443506 allocations
data("speed", "x86", "libxml_dom", "data/vod.xml", 28.9276);
data("memory", "x86", "libxml_dom", "data/vod.xml", 21.6281);
// libxml_sax: 8.64 us/byte, 110 Mb/s, 4108 Kb, 29 allocations
data("speed", "x86", "libxml_sax", "data/vod.xml", 8.63797);
// msxml_dom: 36.91 us/byte, 26 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "msxml_dom", "data/vod.xml", 36.9121);
data("memory", "x86", "msxml_dom", "data/vod.xml", 4.33769);
// msxml_sax: 15.21 us/byte, 63 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "msxml_sax", "data/vod.xml", 15.2093);
// pugixml: 1.84 us/byte, 519 Mb/s, 6496 Kb, 137 allocations
data("speed", "x86", "pugixml", "data/vod.xml", 1.83675);
data("memory", "x86", "pugixml", "data/vod.xml", 6.34441);
// pugixml_compact: 3.13 us/byte, 304 Mb/s, 3959 Kb, 62 allocations
data("speed", "x86", "pugixml_compact", "data/vod.xml", 3.13358);
data("memory", "x86", "pugixml_compact", "data/vod.xml", 3.86705);
// pugxml: 17.75 us/byte, 54 Mb/s, 12007 Kb, 382832 allocations
data("speed", "x86", "pugxml", "data/vod.xml", 17.7471);
data("memory", "x86", "pugxml", "data/vod.xml", 11.7256);
// qtxml_dom: 65.36 us/byte, 15 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "qtxml_dom", "data/vod.xml", 65.3551);
data("memory", "x86", "qtxml_dom", "data/vod.xml", 17.0858);
// qtxml_sax: 41.03 us/byte, 23 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "qtxml_sax", "data/vod.xml", 41.0278);
// rapidxml: 2.08 us/byte, 458 Mb/s, 8598 Kb, 102 allocations
data("speed", "x86", "rapidxml", "data/vod.xml", 2.08312);
data("memory", "x86", "rapidxml", "data/vod.xml", 8.39672);
// tinyxml: 69.92 us/byte, 14 Mb/s, 14663 Kb, 573268 allocations
data("speed", "x86", "tinyxml", "data/vod.xml", 69.9188);
data("memory", "x86", "tinyxml", "data/vod.xml", 14.3194);
// tinyxml2: 18.70 us/byte, 51 Mb/s, 8970 Kb, 1731 allocations
data("speed", "x86", "tinyxml2", "data/vod.xml", 18.7048);
data("memory", "x86", "tinyxml2", "data/vod.xml", 8.76072);
// vtdxml: 5.57 us/byte, 171 Mb/s, 3589 Kb, 49 allocations
data("speed", "x86", "vtdxml", "data/vod.xml", 5.57234);
data("memory", "x86", "vtdxml", "data/vod.xml", 3.50551);
// xerces_dom: 25.16 us/byte, 38 Mb/s, 20271 Kb, 785 allocations
data("speed", "x86", "xerces_dom", "data/vod.xml", 25.1637);
data("memory", "x86", "xerces_dom", "data/vod.xml", 19.7968);
// xerces_sax: 15.51 us/byte, 61 Mb/s, 215 Kb, 415 allocations
data("speed", "x86", "xerces_sax", "data/vod.xml", 15.5105);
// xmllite: 10.06 us/byte, 95 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "xmllite", "data/vod.xml", 10.0633);
// xmlparser: 34.58 us/byte, 28 Mb/s, 9482 Kb, 482180 allocations
data("speed", "x86", "xmlparser", "data/vod.xml", 34.5822);
data("memory", "x86", "xmlparser", "data/vod.xml", 9.2603);

// data/xmark.xml
// asmxml: 1.35 us/byte, 708 Mb/s, 19066 Kb, 557 allocations
data("speed", "x86", "asmxml", "data/xmark.xml", 1.34653);
data("memory", "x86", "asmxml", "data/xmark.xml", 18.6195);
// cmarkup: 5.25 us/byte, 182 Mb/s, 29338 Kb, 10 allocations
data("speed", "x86", "cmarkup", "data/xmark.xml", 5.25097);
data("memory", "x86", "cmarkup", "data/xmark.xml", 28.6508);
// expat: 6.51 us/byte, 146 Mb/s, 16393 Kb, 118 allocations
data("speed", "x86", "expat", "data/xmark.xml", 6.5119);
// irrxml: 17.86 us/byte, 53 Mb/s, 11604 Kb, 1583123 allocations
data("speed", "x86", "irrxml", "data/xmark.xml", 17.864);
// ixmlparser_dom: 10.11 us/byte, 94 Mb/s, 22553 Kb, 40 allocations
data("speed", "x86", "ixmlparser_dom", "data/xmark.xml", 10.1106);
data("memory", "x86", "ixmlparser_dom", "data/xmark.xml", 22.025);
// ixmlparser_pull: 7.04 us/byte, 136 Mb/s, 1024 Kb, 2 allocations
data("speed", "x86", "ixmlparser_pull", "data/xmark.xml", 7.03691);
// libxml_dom: 13.94 us/byte, 68 Mb/s, 68957 Kb, 1109077 allocations
data("speed", "x86", "libxml_dom", "data/xmark.xml", 13.9369);
data("memory", "x86", "libxml_dom", "data/xmark.xml", 67.341);
// libxml_sax: 4.30 us/byte, 222 Mb/s, 16399 Kb, 38303 allocations
data("speed", "x86", "libxml_sax", "data/xmark.xml", 4.30183);
// msxml_dom: 22.26 us/byte, 43 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "msxml_dom", "data/xmark.xml", 22.2641);
data("memory", "x86", "msxml_dom", "data/xmark.xml", 23.5258);
// msxml_sax: 12.36 us/byte, 77 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "msxml_sax", "data/xmark.xml", 12.3572);
// pugixml: 1.25 us/byte, 760 Mb/s, 21319 Kb, 304 allocations
data("speed", "x86", "pugixml", "data/xmark.xml", 1.25456);
data("memory", "x86", "pugixml", "data/xmark.xml", 20.8203);
// pugixml_compact: 1.80 us/byte, 531 Mb/s, 15540 Kb, 130 allocations
data("speed", "x86", "pugixml_compact", "data/xmark.xml", 1.79689);
data("memory", "x86", "pugixml_compact", "data/xmark.xml", 15.1764);
// pugxml: 10.60 us/byte, 90 Mb/s, 32857 Kb, 866856 allocations
data("speed", "x86", "pugxml", "data/xmark.xml", 10.5982);
data("memory", "x86", "pugxml", "data/xmark.xml", 32.0873);
// qtxml_dom: 47.09 us/byte, 20 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "qtxml_dom", "data/xmark.xml", 47.0869);
data("memory", "x86", "qtxml_dom", "data/xmark.xml", 67.7691);
// qtxml_sax: 29.73 us/byte, 32 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "qtxml_sax", "data/xmark.xml", 29.735);
// rapidxml: 1.44 us/byte, 662 Mb/s, 26000 Kb, 226 allocations
data("speed", "x86", "rapidxml", "data/xmark.xml", 1.44153);
data("memory", "x86", "rapidxml", "data/xmark.xml", 25.3913);
// tinyxml: 56.00 us/byte, 17 Mb/s, 42904 Kb, 1596774 allocations
data("speed", "x86", "tinyxml", "data/xmark.xml", 55.9953);
data("memory", "x86", "tinyxml", "data/xmark.xml", 41.899);
// tinyxml2: 8.96 us/byte, 106 Mb/s, 27063 Kb, 3904 allocations
data("speed", "x86", "tinyxml2", "data/xmark.xml", 8.95739);
data("memory", "x86", "tinyxml2", "data/xmark.xml", 26.4292);
// vtdxml: 4.97 us/byte, 192 Mb/s, 14874 Kb, 63 allocations
data("speed", "x86", "vtdxml", "data/xmark.xml", 4.96544);
data("memory", "x86", "vtdxml", "data/xmark.xml", 14.5254);
// xerces_dom: 13.43 us/byte, 71 Mb/s, 62551 Kb, 18608 allocations
data("speed", "x86", "xerces_dom", "data/xmark.xml", 13.432);
data("memory", "x86", "xerces_dom", "data/xmark.xml", 61.085);
// xerces_sax: 8.24 us/byte, 116 Mb/s, 249 Kb, 598 allocations
data("speed", "x86", "xerces_sax", "data/xmark.xml", 8.24084);
// xmllite: 5.11 us/byte, 187 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "xmllite", "data/xmark.xml", 5.11017);
// xmlparser: 23.45 us/byte, 41 Mb/s, 27426 Kb, 1169012 allocations
data("speed", "x86", "xmlparser", "data/xmark.xml", 23.4473);
data("memory", "x86", "xmlparser", "data/xmark.xml", 26.7836);

// data/house.dae
// cmarkup: 3.63 us/byte, 263 Mb/s, 15667 Kb, 17 allocations
data("speed", "x64", "cmarkup", "data/house.dae", 3.62823);
data("memory", "x64", "cmarkup", "data/house.dae", 15.3);
// expat: 1.61 us/byte, 593 Mb/s, 8208 Kb, 164 allocations
data("speed", "x64", "expat", "data/house.dae", 1.60904);
// irrxml: 6.97 us/byte, 137 Mb/s, 6670 Kb, 170531 allocations
data("speed", "x64", "irrxml", "data/house.dae", 6.96587);
// ixmlparser_dom: 6.91 us/byte, 138 Mb/s, 8193 Kb, 14 allocations
data("speed", "x64", "ixmlparser_dom", "data/house.dae", 6.90549);
data("memory", "x64", "ixmlparser_dom", "data/house.dae", 8.00192);
// ixmlparser_pull: 5.75 us/byte, 166 Mb/s, 1024 Kb, 2 allocations
data("speed", "x64", "ixmlparser_pull", "data/house.dae", 5.74871);
// libxml_dom: 2.30 us/byte, 415 Mb/s, 18484 Kb, 57098 allocations
data("speed", "x64", "libxml_dom", "data/house.dae", 2.29856);
data("memory", "x64", "libxml_dom", "data/house.dae", 18.0516);
// libxml_sax: 1.47 us/byte, 648 Mb/s, 8207 Kb, 6964 allocations
data("speed", "x64", "libxml_sax", "data/house.dae", 1.47102);
// msxml_dom: 7.67 us/byte, 124 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "msxml_dom", "data/house.dae", 7.66526);
data("memory", "x64", "msxml_dom", "data/house.dae", 12.699);
// msxml_sax: 7.55 us/byte, 126 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "msxml_sax", "data/house.dae", 7.55236);
// pugixml: 0.56 us/byte, 1706 Mb/s, 7004 Kb, 24 allocations
data("speed", "x64", "pugixml", "data/house.dae", 0.559162);
data("memory", "x64", "pugixml", "data/house.dae", 6.84064);
// pugixml_compact: 0.60 us/byte, 1592 Mb/s, 6474 Kb, 13 allocations
data("speed", "x64", "pugixml_compact", "data/house.dae", 0.599084);
data("memory", "x64", "pugixml_compact", "data/house.dae", 6.32299);
// pugxml: 3.43 us/byte, 278 Mb/s, 7578 Kb, 37428 allocations
data("speed", "x64", "pugxml", "data/house.dae", 3.42802);
data("memory", "x64", "pugxml", "data/house.dae", 7.40063);
// qtxml_dom: 23.20 us/byte, 41 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "qtxml_dom", "data/house.dae", 23.2028);
data("memory", "x64", "qtxml_dom", "data/house.dae", 21.6588);
// qtxml_sax: 19.42 us/byte, 49 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "qtxml_sax", "data/house.dae", 19.4224);
// rapidxml: 0.74 us/byte, 1283 Mb/s, 7290 Kb, 17 allocations
data("speed", "x64", "rapidxml", "data/house.dae", 0.743106);
data("memory", "x64", "rapidxml", "data/house.dae", 7.11984);
// tinyxml: 26.65 us/byte, 36 Mb/s, 11297 Kb, 66621 allocations
data("speed", "x64", "tinyxml", "data/house.dae", 26.6477);
data("memory", "x64", "tinyxml", "data/house.dae", 11.0324);
// tinyxml2: 2.15 us/byte, 444 Mb/s, 7478 Kb, 316 allocations
data("speed", "x64", "tinyxml2", "data/house.dae", 2.14954);
data("memory", "x64", "tinyxml2", "data/house.dae", 7.30319);
// vtdxml: 3.87 us/byte, 246 Mb/s, 6539 Kb, 24 allocations
data("speed", "x64", "vtdxml", "data/house.dae", 3.87382);
data("memory", "x64", "vtdxml", "data/house.dae", 6.38622);
// xerces_dom: 2.99 us/byte, 319 Mb/s, 16906 Kb, 5523 allocations
data("speed", "x64", "xerces_dom", "data/house.dae", 2.98949);
data("memory", "x64", "xerces_dom", "data/house.dae", 16.51);
// xerces_sax: 2.34 us/byte, 408 Mb/s, 874 Kb, 739 allocations
data("speed", "x64", "xerces_sax", "data/house.dae", 2.33549);
// xmllite: 1.79 us/byte, 532 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "xmllite", "data/house.dae", 1.79239);
// xmlparser: 12.17 us/byte, 78 Mb/s, 7355 Kb, 48447 allocations
data("speed", "x64", "xmlparser", "data/house.dae", 12.1739);
data("memory", "x64", "xmlparser", "data/house.dae", 7.1833);

// data/English.sml
// cmarkup: 3.95 us/byte, 241 Mb/s, 3430 Kb, 7 allocations
data("speed", "x64", "cmarkup", "data/English.sml", 3.95322);
data("memory", "x64", "cmarkup", "data/English.sml", 3.34978);
// expat: 2.01 us/byte, 474 Mb/s, 2054 Kb, 22 allocations
data("speed", "x64", "expat", "data/English.sml", 2.00995);
// irrxml: 9.89 us/byte, 96 Mb/s, 1379 Kb, 64680 allocations
data("speed", "x64", "irrxml", "data/English.sml", 9.89325);
// ixmlparser_dom: 8.48 us/byte, 112 Mb/s, 4246 Kb, 24 allocations
data("speed", "x64", "ixmlparser_dom", "data/English.sml", 8.48245);
data("memory", "x64", "ixmlparser_dom", "data/English.sml", 4.1472);
// ixmlparser_pull: 6.68 us/byte, 143 Mb/s, 1024 Kb, 2 allocations
data("speed", "x64", "ixmlparser_pull", "data/English.sml", 6.68166);
// libxml_dom: 6.26 us/byte, 152 Mb/s, 6301 Kb, 37253 allocations
data("speed", "x64", "libxml_dom", "data/English.sml", 6.26183);
data("memory", "x64", "libxml_dom", "data/English.sml", 6.15383);
// libxml_sax: 3.36 us/byte, 284 Mb/s, 2062 Kb, 20 allocations
data("speed", "x64", "libxml_sax", "data/English.sml", 3.3629);
// msxml_dom: 11.88 us/byte, 80 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "msxml_dom", "data/English.sml", 11.8788);
data("memory", "x64", "msxml_dom", "data/English.sml", 2.75735);
// msxml_sax: 11.43 us/byte, 83 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "msxml_sax", "data/English.sml", 11.4319);
// pugixml: 0.75 us/byte, 1271 Mb/s, 2495 Kb, 36 allocations
data("speed", "x64", "pugixml", "data/English.sml", 0.750153);
data("memory", "x64", "pugixml", "data/English.sml", 2.43734);
// pugixml_compact: 1.11 us/byte, 856 Mb/s, 1660 Kb, 16 allocations
data("speed", "x64", "pugixml_compact", "data/English.sml", 1.11457);
data("memory", "x64", "pugixml_compact", "data/English.sml", 1.62154);
// pugxml: 10.55 us/byte, 90 Mb/s, 3511 Kb, 47830 allocations
data("speed", "x64", "pugxml", "data/English.sml", 10.5475);
data("memory", "x64", "pugxml", "data/English.sml", 3.4295);
// qtxml_dom: 29.27 us/byte, 33 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "qtxml_dom", "data/English.sml", 29.2724);
data("memory", "x64", "qtxml_dom", "data/English.sml", 6.67031);
// qtxml_sax: 20.20 us/byte, 47 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "qtxml_sax", "data/English.sml", 20.1999);
// rapidxml: 1.02 us/byte, 938 Mb/s, 3036 Kb, 27 allocations
data("speed", "x64", "rapidxml", "data/English.sml", 1.01624);
data("memory", "x64", "rapidxml", "data/English.sml", 2.96553);
// tinyxml: 36.10 us/byte, 26 Mb/s, 4934 Kb, 100500 allocations
data("speed", "x64", "tinyxml", "data/English.sml", 36.0993);
data("memory", "x64", "tinyxml", "data/English.sml", 4.8186);
// tinyxml2: 3.85 us/byte, 248 Mb/s, 3142 Kb, 456 allocations
data("speed", "x64", "tinyxml2", "data/English.sml", 3.85184);
data("memory", "x64", "tinyxml2", "data/English.sml", 3.06911);
// vtdxml: 4.19 us/byte, 228 Mb/s, 1677 Kb, 28 allocations
data("speed", "x64", "vtdxml", "data/English.sml", 4.18764);
data("memory", "x64", "vtdxml", "data/English.sml", 1.63785);
// xerces_dom: 4.85 us/byte, 197 Mb/s, 5748 Kb, 3278 allocations
data("speed", "x64", "xerces_dom", "data/English.sml", 4.85133);
data("memory", "x64", "xerces_dom", "data/English.sml", 5.61424);
// xerces_sax: 3.39 us/byte, 281 Mb/s, 371 Kb, 213 allocations
data("speed", "x64", "xerces_sax", "data/English.sml", 3.39122);
// xmllite: 3.10 us/byte, 308 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "xmllite", "data/English.sml", 3.09986);
// xmlparser: 17.89 us/byte, 53 Mb/s, 3136 Kb, 60082 allocations
data("speed", "x64", "xmlparser", "data/English.sml", 17.8913);
data("memory", "x64", "xmlparser", "data/English.sml", 3.06335);

// data/Russian.sml
// cmarkup: 3.74 us/byte, 255 Mb/s, 6386 Kb, 7 allocations
data("speed", "x64", "cmarkup", "data/Russian.sml", 3.74158);
data("memory", "x64", "cmarkup", "data/Russian.sml", 6.2364);
// expat: 3.49 us/byte, 273 Mb/s, 4102 Kb, 22 allocations
data("speed", "x64", "expat", "data/Russian.sml", 3.49124);
// irrxml: 7.31 us/byte, 130 Mb/s, 2567 Kb, 61129 allocations
data("speed", "x64", "irrxml", "data/Russian.sml", 7.30836);
// ixmlparser_dom: 6.26 us/byte, 152 Mb/s, 5270 Kb, 25 allocations
data("speed", "x64", "ixmlparser_dom", "data/Russian.sml", 6.26092);
data("memory", "x64", "ixmlparser_dom", "data/Russian.sml", 5.14722);
// ixmlparser_pull: 4.78 us/byte, 200 Mb/s, 1024 Kb, 2 allocations
data("speed", "x64", "ixmlparser_pull", "data/Russian.sml", 4.77822);
// libxml_dom: 14.43 us/byte, 66 Mb/s, 10646 Kb, 39784 allocations
data("speed", "x64", "libxml_dom", "data/Russian.sml", 14.4279);
data("memory", "x64", "libxml_dom", "data/Russian.sml", 10.3973);
// libxml_sax: 12.40 us/byte, 77 Mb/s, 4110 Kb, 20 allocations
data("speed", "x64", "libxml_sax", "data/Russian.sml", 12.4);
// msxml_dom: 8.72 us/byte, 109 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "msxml_dom", "data/Russian.sml", 8.72026);
data("memory", "x64", "msxml_dom", "data/Russian.sml", 5.11354);
// msxml_sax: 10.13 us/byte, 94 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "msxml_sax", "data/Russian.sml", 10.1279);
// pugixml: 0.63 us/byte, 1513 Mb/s, 3613 Kb, 34 allocations
data("speed", "x64", "pugixml", "data/Russian.sml", 0.630327);
data("memory", "x64", "pugixml", "data/Russian.sml", 3.52925);
// pugixml_compact: 0.81 us/byte, 1180 Mb/s, 2810 Kb, 15 allocations
data("speed", "x64", "pugixml_compact", "data/Russian.sml", 0.808061);
data("memory", "x64", "pugixml_compact", "data/Russian.sml", 2.74493);
// pugxml: 5.61 us/byte, 170 Mb/s, 4544 Kb, 44471 allocations
data("speed", "x64", "pugxml", "data/Russian.sml", 5.61179);
data("memory", "x64", "pugxml", "data/Russian.sml", 4.43773);
// qtxml_dom: 24.70 us/byte, 39 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "qtxml_dom", "data/Russian.sml", 24.6953);
data("memory", "x64", "qtxml_dom", "data/Russian.sml", 6.69923);
// qtxml_sax: 12.02 us/byte, 79 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "qtxml_sax", "data/Russian.sml", 12.0204);
// rapidxml: 0.88 us/byte, 1084 Mb/s, 4090 Kb, 25 allocations
data("speed", "x64", "rapidxml", "data/Russian.sml", 0.880005);
data("memory", "x64", "rapidxml", "data/Russian.sml", 3.99509);
// tinyxml: 21.65 us/byte, 44 Mb/s, 6452 Kb, 95136 allocations
data("speed", "x64", "tinyxml", "data/Russian.sml", 21.653);
data("memory", "x64", "tinyxml", "data/Russian.sml", 6.30153);
// tinyxml2: 2.66 us/byte, 358 Mb/s, 4200 Kb, 424 allocations
data("speed", "x64", "tinyxml2", "data/Russian.sml", 2.66307);
data("memory", "x64", "tinyxml2", "data/Russian.sml", 4.1022);
// vtdxml: 5.38 us/byte, 177 Mb/s, 2859 Kb, 28 allocations
data("speed", "x64", "vtdxml", "data/Russian.sml", 5.37945);
data("memory", "x64", "vtdxml", "data/Russian.sml", 2.79249);
// xerces_dom: 6.29 us/byte, 152 Mb/s, 5981 Kb, 3240 allocations
data("speed", "x64", "xerces_dom", "data/Russian.sml", 6.28643);
data("memory", "x64", "xerces_dom", "data/Russian.sml", 5.84166);
// xerces_sax: 5.50 us/byte, 173 Mb/s, 368 Kb, 212 allocations
data("speed", "x64", "xerces_sax", "data/Russian.sml", 5.50159);
// xmllite: 5.03 us/byte, 190 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "xmllite", "data/Russian.sml", 5.02692);
// xmlparser: 11.03 us/byte, 86 Mb/s, 4195 Kb, 55836 allocations
data("speed", "x64", "xmlparser", "data/Russian.sml", 11.0256);
data("memory", "x64", "xmlparser", "data/Russian.sml", 4.09725);

// data/blog.xml
// cmarkup: 4.55 us/byte, 210 Mb/s, 3261 Kb, 9 allocations
data("speed", "x64", "cmarkup", "data/blog.xml", 4.546);
data("memory", "x64", "cmarkup", "data/blog.xml", 3.18461);
// expat: 4.53 us/byte, 211 Mb/s, 2061 Kb, 137 allocations
data("speed", "x64", "expat", "data/blog.xml", 4.52838);
// irrxml: 18.65 us/byte, 51 Mb/s, 1323 Kb, 171511 allocations
data("speed", "x64", "irrxml", "data/blog.xml", 18.6483);
// ixmlparser_dom: 8.86 us/byte, 108 Mb/s, 4109 Kb, 17 allocations
data("speed", "x64", "ixmlparser_dom", "data/blog.xml", 8.85583);
data("memory", "x64", "ixmlparser_dom", "data/blog.xml", 4.01302);
// ixmlparser_pull: 6.65 us/byte, 144 Mb/s, 1024 Kb, 2 allocations
data("speed", "x64", "ixmlparser_pull", "data/blog.xml", 6.64531);
// libxml_dom: 9.54 us/byte, 100 Mb/s, 9096 Kb, 79871 allocations
data("speed", "x64", "libxml_dom", "data/blog.xml", 9.53616);
data("memory", "x64", "libxml_dom", "data/blog.xml", 8.88346);
// libxml_sax: 3.83 us/byte, 249 Mb/s, 2080 Kb, 5238 allocations
data("speed", "x64", "libxml_sax", "data/blog.xml", 3.82754);
// msxml_dom: 18.84 us/byte, 51 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "msxml_dom", "data/blog.xml", 18.8387);
data("memory", "x64", "msxml_dom", "data/blog.xml", 2.63131);
// msxml_sax: 11.29 us/byte, 84 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "msxml_sax", "data/blog.xml", 11.2884);
// pugixml: 1.12 us/byte, 854 Mb/s, 2780 Kb, 47 allocations
data("speed", "x64", "pugixml", "data/blog.xml", 1.11615);
data("memory", "x64", "pugixml", "data/blog.xml", 2.71542);
// pugixml_compact: 1.46 us/byte, 651 Mb/s, 1656 Kb, 18 allocations
data("speed", "x64", "pugixml_compact", "data/blog.xml", 1.46481);
data("memory", "x64", "pugixml_compact", "data/blog.xml", 1.61741);
// pugxml: 7.84 us/byte, 122 Mb/s, 3877 Kb, 64805 allocations
data("speed", "x64", "pugxml", "data/blog.xml", 7.83709);
data("memory", "x64", "pugxml", "data/blog.xml", 3.78656);
// qtxml_dom: 38.42 us/byte, 25 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "qtxml_dom", "data/blog.xml", 38.4241);
data("memory", "x64", "qtxml_dom", "data/blog.xml", 9.39754);
// qtxml_sax: 24.86 us/byte, 38 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "qtxml_sax", "data/blog.xml", 24.863);
// rapidxml: 1.36 us/byte, 699 Mb/s, 3480 Kb, 35 allocations
data("speed", "x64", "rapidxml", "data/blog.xml", 1.36451);
data("memory", "x64", "rapidxml", "data/blog.xml", 3.39888);
// tinyxml: 36.41 us/byte, 26 Mb/s, 5925 Kb, 138821 allocations
data("speed", "x64", "tinyxml", "data/blog.xml", 36.4145);
data("memory", "x64", "tinyxml", "data/blog.xml", 5.78624);
// tinyxml2: 5.32 us/byte, 179 Mb/s, 3676 Kb, 611 allocations
data("speed", "x64", "tinyxml2", "data/blog.xml", 5.32119);
data("memory", "x64", "tinyxml2", "data/blog.xml", 3.59007);
// vtdxml: 4.71 us/byte, 203 Mb/s, 1594 Kb, 28 allocations
data("speed", "x64", "vtdxml", "data/blog.xml", 4.70751);
data("memory", "x64", "vtdxml", "data/blog.xml", 1.55725);
// xerces_dom: 10.14 us/byte, 94 Mb/s, 8481 Kb, 3592 allocations
data("speed", "x64", "xerces_dom", "data/blog.xml", 10.1377);
data("memory", "x64", "xerces_dom", "data/blog.xml", 8.28272);
// xerces_sax: 7.00 us/byte, 136 Mb/s, 260 Kb, 510 allocations
data("speed", "x64", "xerces_sax", "data/blog.xml", 7.00134);
// xmllite: 3.59 us/byte, 265 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "xmllite", "data/blog.xml", 3.59232);
// xmlparser: 19.75 us/byte, 48 Mb/s, 3463 Kb, 91388 allocations
data("speed", "x64", "xmlparser", "data/blog.xml", 19.7453);
data("memory", "x64", "xmlparser", "data/blog.xml", 3.38209);

// data/cathedral.xml
// cmarkup: 7.09 us/byte, 135 Mb/s, 2946 Kb, 8 allocations
data("speed", "x64", "cmarkup", "data/cathedral.xml", 7.09028);
data("memory", "x64", "cmarkup", "data/cathedral.xml", 2.87762);
// expat: 15.70 us/byte, 61 Mb/s, 1030 Kb, 23 allocations
data("speed", "x64", "expat", "data/cathedral.xml", 15.6961);
// irrxml: 65.05 us/byte, 15 Mb/s, 907 Kb, 657154 allocations
data("speed", "x64", "irrxml", "data/cathedral.xml", 65.0457);
// ixmlparser_dom: 10.23 us/byte, 93 Mb/s, 5195 Kb, 23 allocations
data("speed", "x64", "ixmlparser_dom", "data/cathedral.xml", 10.2269);
data("memory", "x64", "ixmlparser_dom", "data/cathedral.xml", 5.07397);
// ixmlparser_pull: 5.47 us/byte, 174 Mb/s, 1024 Kb, 2 allocations
data("speed", "x64", "ixmlparser_pull", "data/cathedral.xml", 5.46644);
// libxml_dom: 29.54 us/byte, 32 Mb/s, 17432 Kb, 225750 allocations
data("speed", "x64", "libxml_dom", "data/cathedral.xml", 29.5389);
data("memory", "x64", "libxml_dom", "data/cathedral.xml", 17.0238);
// libxml_sax: 10.99 us/byte, 87 Mb/s, 1037 Kb, 30115 allocations
data("speed", "x64", "libxml_sax", "data/cathedral.xml", 10.9854);
// msxml_dom: 37.78 us/byte, 25 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "msxml_dom", "data/cathedral.xml", 37.7766);
data("memory", "x64", "msxml_dom", "data/cathedral.xml", 1.82484);
// msxml_sax: 11.52 us/byte, 83 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "msxml_sax", "data/cathedral.xml", 11.5227);
// pugixml: 1.23 us/byte, 777 Mb/s, 3379 Kb, 78 allocations
data("speed", "x64", "pugixml", "data/cathedral.xml", 1.22756);
data("memory", "x64", "pugixml", "data/cathedral.xml", 3.30012);
// pugixml_compact: 2.04 us/byte, 467 Mb/s, 1518 Kb, 23 allocations
data("speed", "x64", "pugixml_compact", "data/cathedral.xml", 2.04179);
data("memory", "x64", "pugixml_compact", "data/cathedral.xml", 1.48266);
// pugxml: 17.05 us/byte, 56 Mb/s, 5528 Kb, 136702 allocations
data("speed", "x64", "pugxml", "data/cathedral.xml", 17.0467);
data("memory", "x64", "pugxml", "data/cathedral.xml", 5.39868);
// qtxml_dom: 73.82 us/byte, 13 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "qtxml_dom", "data/cathedral.xml", 73.8215);
data("memory", "x64", "qtxml_dom", "data/cathedral.xml", 18.294);
// qtxml_sax: 44.14 us/byte, 22 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "qtxml_sax", "data/cathedral.xml", 44.1388);
// rapidxml: 1.62 us/byte, 590 Mb/s, 4428 Kb, 56 allocations
data("speed", "x64", "rapidxml", "data/cathedral.xml", 1.61555);
data("memory", "x64", "rapidxml", "data/cathedral.xml", 4.32455);
// tinyxml: 49.33 us/byte, 19 Mb/s, 9327 Kb, 240788 allocations
data("speed", "x64", "tinyxml", "data/cathedral.xml", 49.3297);
data("memory", "x64", "tinyxml", "data/cathedral.xml", 9.10884);
// tinyxml2: 14.31 us/byte, 67 Mb/s, 5096 Kb, 1067 allocations
data("speed", "x64", "tinyxml2", "data/cathedral.xml", 14.3096);
data("memory", "x64", "tinyxml2", "data/cathedral.xml", 4.97752);
// vtdxml: 5.88 us/byte, 162 Mb/s, 1798 Kb, 58 allocations
data("speed", "x64", "vtdxml", "data/cathedral.xml", 5.87701);
data("memory", "x64", "vtdxml", "data/cathedral.xml", 1.75605);
// xerces_dom: 27.31 us/byte, 35 Mb/s, 14584 Kb, 259 allocations
data("speed", "x64", "xerces_dom", "data/cathedral.xml", 27.3137);
data("memory", "x64", "xerces_dom", "data/cathedral.xml", 14.243);
// xerces_sax: 16.89 us/byte, 56 Mb/s, 294 Kb, 221 allocations
data("speed", "x64", "xerces_sax", "data/cathedral.xml", 16.8882);
// xmllite: 6.22 us/byte, 153 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "xmllite", "data/cathedral.xml", 6.22353);
// xmlparser: 22.31 us/byte, 43 Mb/s, 5167 Kb, 170699 allocations
data("speed", "x64", "xmlparser", "data/cathedral.xml", 22.309);
data("memory", "x64", "xmlparser", "data/cathedral.xml", 5.04624);

// data/employees-big.xml
// cmarkup: 6.60 us/byte, 145 Mb/s, 28670 Kb, 10 allocations
data("speed", "x64", "cmarkup", "data/employees-big.xml", 6.59584);
data("memory", "x64", "cmarkup", "data/employees-big.xml", 27.999);
// expat: 11.52 us/byte, 83 Mb/s, 16390 Kb, 23 allocations
data("speed", "x64", "expat", "data/employees-big.xml", 11.5176);
// irrxml: 41.64 us/byte, 23 Mb/s, 10240 Kb, 3963353 allocations
data("speed", "x64", "irrxml", "data/employees-big.xml", 41.6412);
// ixmlparser_dom: 10.17 us/byte, 94 Mb/s, 36441 Kb, 59 allocations
data("speed", "x64", "ixmlparser_dom", "data/employees-big.xml", 10.1731);
data("memory", "x64", "ixmlparser_dom", "data/employees-big.xml", 35.5871);
// ixmlparser_pull: 5.80 us/byte, 165 Mb/s, 1024 Kb, 2 allocations
data("speed", "x64", "ixmlparser_pull", "data/employees-big.xml", 5.79525);
// libxml_dom: 22.81 us/byte, 42 Mb/s, 153842 Kb, 1777509 allocations
data("speed", "x64", "libxml_dom", "data/employees-big.xml", 22.8078);
data("memory", "x64", "libxml_dom", "data/employees-big.xml", 150.237);
// libxml_sax: 6.96 us/byte, 137 Mb/s, 16401 Kb, 84088 allocations
data("speed", "x64", "libxml_sax", "data/employees-big.xml", 6.96193);
// msxml_dom: 41.70 us/byte, 23 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "msxml_dom", "data/employees-big.xml", 41.6956);
data("memory", "x64", "msxml_dom", "data/employees-big.xml", 20.6788);
// msxml_sax: 11.22 us/byte, 85 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "msxml_sax", "data/employees-big.xml", 11.2196);
// pugixml: 1.49 us/byte, 642 Mb/s, 40718 Kb, 950 allocations
data("speed", "x64", "pugixml", "data/employees-big.xml", 1.48641);
data("memory", "x64", "pugixml", "data/employees-big.xml", 39.764);
// pugixml_compact: 2.31 us/byte, 413 Mb/s, 16850 Kb, 202 allocations
data("speed", "x64", "pugixml_compact", "data/employees-big.xml", 2.30906);
data("memory", "x64", "pugixml_compact", "data/employees-big.xml", 16.4555);
// pugxml: 29.01 us/byte, 33 Mb/s, 65970 Kb, 1369156 allocations
data("speed", "x64", "pugxml", "data/employees-big.xml", 29.0147);
data("memory", "x64", "pugxml", "data/employees-big.xml", 64.4239);
// qtxml_dom: 56.57 us/byte, 17 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "qtxml_dom", "data/employees-big.xml", 56.5661);
data("memory", "x64", "qtxml_dom", "data/employees-big.xml", 128.694);
// qtxml_sax: 31.86 us/byte, 30 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "qtxml_sax", "data/employees-big.xml", 31.857);
// rapidxml: 1.74 us/byte, 547 Mb/s, 55385 Kb, 706 allocations
data("speed", "x64", "rapidxml", "data/employees-big.xml", 1.74466);
data("memory", "x64", "rapidxml", "data/employees-big.xml", 54.0872);
// tinyxml: 50.16 us/byte, 19 Mb/s, 95377 Kb, 2450061 allocations
data("speed", "x64", "tinyxml", "data/employees-big.xml", 50.162);
data("memory", "x64", "tinyxml", "data/employees-big.xml", 93.1417);
// tinyxml2: 15.21 us/byte, 63 Mb/s, 58921 Kb, 12266 allocations
data("speed", "x64", "tinyxml2", "data/employees-big.xml", 15.2102);
data("memory", "x64", "tinyxml2", "data/employees-big.xml", 57.5408);
// vtdxml: 5.01 us/byte, 190 Mb/s, 16995 Kb, 603 allocations
data("speed", "x64", "vtdxml", "data/employees-big.xml", 5.01358);
data("memory", "x64", "vtdxml", "data/employees-big.xml", 16.5972);
// xerces_dom: 20.41 us/byte, 47 Mb/s, 125521 Kb, 487 allocations
data("speed", "x64", "xerces_dom", "data/employees-big.xml", 20.411);
data("memory", "x64", "xerces_dom", "data/employees-big.xml", 122.579);
// xerces_sax: 12.35 us/byte, 77 Mb/s, 907 Kb, 233 allocations
data("speed", "x64", "xerces_sax", "data/employees-big.xml", 12.3527);
// xmllite: 6.69 us/byte, 142 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "xmllite", "data/employees-big.xml", 6.69262);
// xmlparser: 26.89 us/byte, 35 Mb/s, 55210 Kb, 1898880 allocations
data("speed", "x64", "xmlparser", "data/employees-big.xml", 26.8875);
data("memory", "x64", "xmlparser", "data/employees-big.xml", 53.9167);

// data/terrover.xml
// cmarkup: 5.99 us/byte, 159 Mb/s, 38444 Kb, 12 allocations
data("speed", "x64", "cmarkup", "data/terrover.xml", 5.98534);
data("memory", "x64", "cmarkup", "data/terrover.xml", 37.5439);
// expat: 12.91 us/byte, 74 Mb/s, 16402 Kb, 132 allocations
data("speed", "x64", "expat", "data/terrover.xml", 12.9135);
// irrxml: 31.44 us/byte, 30 Mb/s, 16165 Kb, 4961420 allocations
data("speed", "x64", "irrxml", "data/terrover.xml", 31.4401);
// ixmlparser_dom: 8.41 us/byte, 113 Mb/s, 35859 Kb, 49 allocations
data("speed", "x64", "ixmlparser_dom", "data/terrover.xml", 8.40951);
data("memory", "x64", "ixmlparser_dom", "data/terrover.xml", 35.0195);
// ixmlparser_pull: 5.84 us/byte, 163 Mb/s, 1024 Kb, 2 allocations
data("speed", "x64", "ixmlparser_pull", "data/terrover.xml", 5.84213);
// libxml_dom: 14.99 us/byte, 64 Mb/s, 148443 Kb, 1652648 allocations
data("speed", "x64", "libxml_dom", "data/terrover.xml", 14.9915);
data("memory", "x64", "libxml_dom", "data/terrover.xml", 144.964);
// libxml_sax: 6.10 us/byte, 156 Mb/s, 16403 Kb, 230832 allocations
data("speed", "x64", "libxml_sax", "data/terrover.xml", 6.09607);
// msxml_dom: 36.70 us/byte, 26 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "msxml_dom", "data/terrover.xml", 36.702);
data("memory", "x64", "msxml_dom", "data/terrover.xml", 32.4265);
// msxml_sax: 9.42 us/byte, 101 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "msxml_sax", "data/terrover.xml", 9.419);
// pugixml: 0.94 us/byte, 1015 Mb/s, 35420 Kb, 601 allocations
data("speed", "x64", "pugixml", "data/terrover.xml", 0.939728);
data("memory", "x64", "pugixml", "data/terrover.xml", 34.59);
// pugixml_compact: 1.36 us/byte, 701 Mb/s, 21060 Kb, 156 allocations
data("speed", "x64", "pugixml_compact", "data/terrover.xml", 1.3613);
data("memory", "x64", "pugixml_compact", "data/terrover.xml", 20.567);
// pugxml: 8.95 us/byte, 107 Mb/s, 51112 Kb, 1031012 allocations
data("speed", "x64", "pugxml", "data/terrover.xml", 8.94631);
data("memory", "x64", "pugxml", "data/terrover.xml", 49.915);
// qtxml_dom: 48.50 us/byte, 20 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "qtxml_dom", "data/terrover.xml", 48.4981);
data("memory", "x64", "qtxml_dom", "data/terrover.xml", 172.664);
// qtxml_sax: 32.12 us/byte, 30 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "qtxml_sax", "data/terrover.xml", 32.1153);
// rapidxml: 1.24 us/byte, 769 Mb/s, 44006 Kb, 436 allocations
data("speed", "x64", "rapidxml", "data/terrover.xml", 1.24031);
data("memory", "x64", "rapidxml", "data/terrover.xml", 42.9746);
// tinyxml: 34.98 us/byte, 27 Mb/s, 84489 Kb, 2131643 allocations
data("speed", "x64", "tinyxml", "data/terrover.xml", 34.9841);
data("memory", "x64", "tinyxml", "data/terrover.xml", 82.5089);
// tinyxml2: 8.18 us/byte, 117 Mb/s, 49103 Kb, 8306 allocations
data("speed", "x64", "tinyxml2", "data/terrover.xml", 8.18213);
data("memory", "x64", "tinyxml2", "data/terrover.xml", 47.9522);
// vtdxml: 5.59 us/byte, 171 Mb/s, 21341 Kb, 55 allocations
data("speed", "x64", "vtdxml", "data/terrover.xml", 5.59125);
data("memory", "x64", "vtdxml", "data/terrover.xml", 20.8415);
// xerces_dom: 21.03 us/byte, 45 Mb/s, 124431 Kb, 7467 allocations
data("speed", "x64", "xerces_dom", "data/terrover.xml", 21.0342);
data("memory", "x64", "xerces_dom", "data/terrover.xml", 121.515);
// xerces_sax: 15.61 us/byte, 61 Mb/s, 261 Kb, 509 allocations
data("speed", "x64", "xerces_sax", "data/terrover.xml", 15.6122);
// xmllite: 3.82 us/byte, 250 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "xmllite", "data/terrover.xml", 3.81973);
// xmlparser: 16.95 us/byte, 56 Mb/s, 48579 Kb, 1234038 allocations
data("speed", "x64", "xmlparser", "data/terrover.xml", 16.9472);
data("memory", "x64", "xmlparser", "data/terrover.xml", 47.4411);

// data/vod.xml
// cmarkup: 7.26 us/byte, 131 Mb/s, 8216 Kb, 12 allocations
data("speed", "x64", "cmarkup", "data/vod.xml", 7.25517);
data("memory", "x64", "cmarkup", "data/vod.xml", 8.02387);
// expat: 12.40 us/byte, 77 Mb/s, 4105 Kb, 71 allocations
data("speed", "x64", "expat", "data/vod.xml", 12.3953);
// irrxml: 24.26 us/byte, 39 Mb/s, 2134 Kb, 461914 allocations
data("speed", "x64", "irrxml", "data/vod.xml", 24.2638);
// ixmlparser_dom: 12.12 us/byte, 79 Mb/s, 11283 Kb, 25 allocations
data("speed", "x64", "ixmlparser_dom", "data/vod.xml", 12.1158);
data("memory", "x64", "ixmlparser_dom", "data/vod.xml", 11.0191);
// ixmlparser_pull: 6.58 us/byte, 145 Mb/s, 1024 Kb, 2 allocations
data("speed", "x64", "ixmlparser_pull", "data/vod.xml", 6.57894);
// libxml_dom: 27.47 us/byte, 35 Mb/s, 38602 Kb, 443506 allocations
data("speed", "x64", "libxml_dom", "data/vod.xml", 27.4728);
data("memory", "x64", "libxml_dom", "data/vod.xml", 37.6982);
// libxml_sax: 8.31 us/byte, 115 Mb/s, 4110 Kb, 29 allocations
data("speed", "x64", "libxml_sax", "data/vod.xml", 8.30681);
// msxml_dom: 32.96 us/byte, 29 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "msxml_dom", "data/vod.xml", 32.9603);
data("memory", "x64", "msxml_dom", "data/vod.xml", 4.31355);
// msxml_sax: 12.23 us/byte, 78 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "msxml_sax", "data/vod.xml", 12.2261);
// pugixml: 1.98 us/byte, 481 Mb/s, 10836 Kb, 272 allocations
data("speed", "x64", "pugixml", "data/vod.xml", 1.98167);
data("memory", "x64", "pugixml", "data/vod.xml", 10.5823);
// pugixml_compact: 2.90 us/byte, 328 Mb/s, 3897 Kb, 61 allocations
data("speed", "x64", "pugixml_compact", "data/vod.xml", 2.90367);
data("memory", "x64", "pugixml_compact", "data/vod.xml", 3.80631);
// pugxml: 15.83 us/byte, 60 Mb/s, 18640 Kb, 382832 allocations
data("speed", "x64", "pugxml", "data/vod.xml", 15.834);
data("memory", "x64", "pugxml", "data/vod.xml", 18.2032);
// qtxml_dom: 59.32 us/byte, 16 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "qtxml_dom", "data/vod.xml", 59.3215);
data("memory", "x64", "qtxml_dom", "data/vod.xml", 27.2374);
// qtxml_sax: 35.05 us/byte, 27 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "qtxml_sax", "data/vod.xml", 35.0499);
// rapidxml: 2.20 us/byte, 433 Mb/s, 15067 Kb, 203 allocations
data("speed", "x64", "rapidxml", "data/vod.xml", 2.20434);
data("memory", "x64", "rapidxml", "data/vod.xml", 14.7148);
// tinyxml: 55.90 us/byte, 17 Mb/s, 25801 Kb, 573268 allocations
data("speed", "x64", "tinyxml", "data/vod.xml", 55.9004);
data("memory", "x64", "tinyxml", "data/vod.xml", 25.1972);
// tinyxml2: 18.05 us/byte, 53 Mb/s, 15845 Kb, 3463 allocations
data("speed", "x64", "tinyxml2", "data/vod.xml", 18.0515);
data("memory", "x64", "tinyxml2", "data/vod.xml", 15.4736);
// vtdxml: 5.49 us/byte, 174 Mb/s, 3590 Kb, 49 allocations
data("speed", "x64", "vtdxml", "data/vod.xml", 5.48598);
data("memory", "x64", "vtdxml", "data/vod.xml", 3.50608);
// xerces_dom: 20.76 us/byte, 46 Mb/s, 32588 Kb, 809 allocations
data("speed", "x64", "xerces_dom", "data/vod.xml", 20.7587);
data("memory", "x64", "xerces_dom", "data/vod.xml", 31.825);
// xerces_sax: 14.05 us/byte, 68 Mb/s, 243 Kb, 415 allocations
data("speed", "x64", "xerces_sax", "data/vod.xml", 14.0536);
// xmllite: 8.87 us/byte, 108 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "xmllite", "data/vod.xml", 8.87043);
// xmlparser: 29.93 us/byte, 32 Mb/s, 15709 Kb, 482180 allocations
data("speed", "x64", "xmlparser", "data/vod.xml", 29.9344);
data("memory", "x64", "xmlparser", "data/vod.xml", 15.341);

// data/xmark.xml
// cmarkup: 5.18 us/byte, 184 Mb/s, 29338 Kb, 10 allocations
data("speed", "x64", "cmarkup", "data/xmark.xml", 5.18089);
data("memory", "x64", "cmarkup", "data/xmark.xml", 28.651);
// expat: 6.06 us/byte, 157 Mb/s, 16398 Kb, 118 allocations
data("speed", "x64", "expat", "data/xmark.xml", 6.0587);
// irrxml: 17.02 us/byte, 56 Mb/s, 11604 Kb, 1583123 allocations
data("speed", "x64", "irrxml", "data/xmark.xml", 17.0197);
// ixmlparser_dom: 9.50 us/byte, 100 Mb/s, 29734 Kb, 47 allocations
data("speed", "x64", "ixmlparser_dom", "data/xmark.xml", 9.49568);
data("memory", "x64", "ixmlparser_dom", "data/xmark.xml", 29.0379);
// ixmlparser_pull: 6.54 us/byte, 146 Mb/s, 1024 Kb, 2 allocations
data("speed", "x64", "ixmlparser_pull", "data/xmark.xml", 6.54021);
// libxml_dom: 12.84 us/byte, 74 Mb/s, 109273 Kb, 1109077 allocations
data("speed", "x64", "libxml_dom", "data/xmark.xml", 12.8449);
data("memory", "x64", "libxml_dom", "data/xmark.xml", 106.713);
// libxml_sax: 4.09 us/byte, 233 Mb/s, 16401 Kb, 38303 allocations
data("speed", "x64", "libxml_sax", "data/xmark.xml", 4.08763);
// msxml_dom: 25.79 us/byte, 37 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "msxml_dom", "data/xmark.xml", 25.7945);
data("memory", "x64", "msxml_dom", "data/xmark.xml", 23.2286);
// msxml_sax: 10.44 us/byte, 91 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "msxml_sax", "data/xmark.xml", 10.445);
// pugixml: 1.21 us/byte, 790 Mb/s, 31059 Kb, 607 allocations
data("speed", "x64", "pugixml", "data/xmark.xml", 1.20792);
data("memory", "x64", "pugixml", "data/xmark.xml", 30.3318);
// pugixml_compact: 1.66 us/byte, 575 Mb/s, 15672 Kb, 130 allocations
data("speed", "x64", "pugixml_compact", "data/xmark.xml", 1.65954);
data("memory", "x64", "pugixml_compact", "data/xmark.xml", 15.3055);
// pugxml: 9.61 us/byte, 99 Mb/s, 47414 Kb, 866856 allocations
data("speed", "x64", "pugxml", "data/xmark.xml", 9.61319);
data("memory", "x64", "pugxml", "data/xmark.xml", 46.3031);
// qtxml_dom: 43.04 us/byte, 22 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "qtxml_dom", "data/xmark.xml", 43.0374);
data("memory", "x64", "qtxml_dom", "data/xmark.xml", 96.834);
// qtxml_sax: 25.42 us/byte, 38 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "qtxml_sax", "data/xmark.xml", 25.4212);
// rapidxml: 1.44 us/byte, 660 Mb/s, 40477 Kb, 452 allocations
data("speed", "x64", "rapidxml", "data/xmark.xml", 1.44408);
data("memory", "x64", "rapidxml", "data/xmark.xml", 39.5288);
// tinyxml: 42.00 us/byte, 23 Mb/s, 68671 Kb, 1596774 allocations
data("speed", "x64", "tinyxml", "data/xmark.xml", 42.0017);
data("memory", "x64", "tinyxml", "data/xmark.xml", 67.0622);
// tinyxml2: 8.75 us/byte, 109 Mb/s, 42621 Kb, 7827 allocations
data("speed", "x64", "tinyxml2", "data/xmark.xml", 8.75425);
data("memory", "x64", "tinyxml2", "data/xmark.xml", 41.623);
// vtdxml: 4.58 us/byte, 208 Mb/s, 14874 Kb, 63 allocations
data("speed", "x64", "vtdxml", "data/xmark.xml", 4.5776);
data("memory", "x64", "vtdxml", "data/xmark.xml", 14.5261);
// xerces_dom: 12.01 us/byte, 79 Mb/s, 91404 Kb, 18664 allocations
data("speed", "x64", "xerces_dom", "data/xmark.xml", 12.0072);
data("memory", "x64", "xerces_dom", "data/xmark.xml", 89.2626);
// xerces_sax: 7.45 us/byte, 128 Mb/s, 288 Kb, 598 allocations
data("speed", "x64", "xerces_sax", "data/xmark.xml", 7.44949);
// xmllite: 4.44 us/byte, 215 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "xmllite", "data/xmark.xml", 4.44122);
// xmlparser: 20.77 us/byte, 46 Mb/s, 41385 Kb, 1169012 allocations
data("speed", "x64", "xmlparser", "data/xmark.xml", 20.768);
data("memory", "x64", "xmlparser", "data/xmark.xml", 40.4151);

} 
