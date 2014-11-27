function benchmark(data) { 
// data/house.dae
// asmxml: 1.03 us/byte, 922 Mb/s, 7559 Kb, 2118 allocations
data("speed", "x86", "asmxml", "data/house.dae", 1.03442);
data("memory", "x86", "asmxml", "data/house.dae", 7.38267);
// cmarkup: 3.73 us/byte, 256 Mb/s, 15666 Kb, 17 allocations
data("speed", "x86", "cmarkup", "data/house.dae", 3.7286);
data("memory", "x86", "cmarkup", "data/house.dae", 15.2997);
// expat: 2.00 us/byte, 477 Mb/s, 8203 Kb, 164 allocations
data("speed", "x86", "expat", "data/house.dae", 1.99992);
// irrxml: 7.75 us/byte, 123 Mb/s, 6670 Kb, 170531 allocations
data("speed", "x86", "irrxml", "data/house.dae", 7.75037);
// ixmlparser_dom: 7.22 us/byte, 132 Mb/s, 8193 Kb, 14 allocations
data("speed", "x86", "ixmlparser_dom", "data/house.dae", 7.2182);
data("memory", "x86", "ixmlparser_dom", "data/house.dae", 8.00116);
// ixmlparser_pull: 6.20 us/byte, 154 Mb/s, 1024 Kb, 2 allocations
data("speed", "x86", "ixmlparser_pull", "data/house.dae", 6.19951);
// libxml_dom: 2.54 us/byte, 376 Mb/s, 16446 Kb, 57098 allocations
data("speed", "x86", "libxml_dom", "data/house.dae", 2.53516);
data("memory", "x86", "libxml_dom", "data/house.dae", 16.0611);
// libxml_sax: 1.51 us/byte, 631 Mb/s, 8205 Kb, 6964 allocations
data("speed", "x86", "libxml_sax", "data/house.dae", 1.51187);
// msxml_dom: 6.97 us/byte, 137 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "msxml_dom", "data/house.dae", 6.96748);
data("memory", "x86", "msxml_dom", "data/house.dae", 41.0391);
// msxml_sax: 9.28 us/byte, 103 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "msxml_sax", "data/house.dae", 9.27724);
// pugixml: 0.55 us/byte, 1737 Mb/s, 6651 Kb, 13 allocations
data("speed", "x86", "pugixml", "data/house.dae", 0.5491);
data("memory", "x86", "pugixml", "data/house.dae", 6.4953);
// pugixml_compact: 0.59 us/byte, 1611 Mb/s, 6466 Kb, 13 allocations
data("speed", "x86", "pugixml_compact", "data/house.dae", 0.591886);
data("memory", "x86", "pugixml_compact", "data/house.dae", 6.31496);
// pugxml: 3.63 us/byte, 263 Mb/s, 7005 Kb, 37428 allocations
data("speed", "x86", "pugxml", "data/house.dae", 3.62563);
data("memory", "x86", "pugxml", "data/house.dae", 6.84106);
// qtxml_dom: 22.38 us/byte, 43 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "qtxml_dom", "data/house.dae", 22.3757);
// qtxml_sax: 18.14 us/byte, 53 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "qtxml_sax", "data/house.dae", 18.1392);
// rapidxml: 0.69 us/byte, 1380 Mb/s, 6778 Kb, 9 allocations
data("speed", "x86", "rapidxml", "data/house.dae", 0.691056);
data("memory", "x86", "rapidxml", "data/house.dae", 6.61939);
// tinyxml: 38.93 us/byte, 24 Mb/s, 10188 Kb, 66621 allocations
data("speed", "x86", "tinyxml", "data/house.dae", 38.9259);
data("memory", "x86", "tinyxml", "data/house.dae", 9.94964);
// tinyxml2: 2.29 us/byte, 417 Mb/s, 6879 Kb, 162 allocations
data("speed", "x86", "tinyxml2", "data/house.dae", 2.28804);
data("memory", "x86", "tinyxml2", "data/house.dae", 6.71816);
// vtdxml: 4.28 us/byte, 223 Mb/s, 6539 Kb, 24 allocations
data("speed", "x86", "vtdxml", "data/house.dae", 4.2773);
data("memory", "x86", "vtdxml", "data/house.dae", 6.38584);
// xerces_dom: 3.43 us/byte, 278 Mb/s, 15260 Kb, 5520 allocations
data("speed", "x86", "xerces_dom", "data/house.dae", 3.42882);
data("memory", "x86", "xerces_dom", "data/house.dae", 14.9027);
// xerces_sax: 2.56 us/byte, 373 Mb/s, 847 Kb, 739 allocations
data("speed", "x86", "xerces_sax", "data/house.dae", 2.55576);
// xmllite: 2.20 us/byte, 434 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "xmllite", "data/house.dae", 2.19873);
// xmlparser: 13.31 us/byte, 72 Mb/s, 6788 Kb, 48447 allocations
data("speed", "x86", "xmlparser", "data/house.dae", 13.3099);
data("memory", "x86", "xmlparser", "data/house.dae", 6.62947);

// data/English.sml
// asmxml: 1.13 us/byte, 844 Mb/s, 2703 Kb, 39 allocations
data("speed", "x86", "asmxml", "data/English.sml", 1.12949);
data("memory", "x86", "asmxml", "data/English.sml", 2.64017);
// cmarkup: 4.20 us/byte, 227 Mb/s, 3430 Kb, 7 allocations
data("speed", "x86", "cmarkup", "data/English.sml", 4.19526);
data("memory", "x86", "cmarkup", "data/English.sml", 3.34965);
// expat: 2.12 us/byte, 449 Mb/s, 2053 Kb, 22 allocations
data("speed", "x86", "expat", "data/English.sml", 2.12351);
// irrxml: 9.62 us/byte, 99 Mb/s, 1379 Kb, 64680 allocations
data("speed", "x86", "irrxml", "data/English.sml", 9.62291);
// ixmlparser_dom: 8.95 us/byte, 107 Mb/s, 3197 Kb, 23 allocations
data("speed", "x86", "ixmlparser_dom", "data/English.sml", 8.95131);
data("memory", "x86", "ixmlparser_dom", "data/English.sml", 3.12243);
// ixmlparser_pull: 7.16 us/byte, 133 Mb/s, 1024 Kb, 2 allocations
data("speed", "x86", "ixmlparser_pull", "data/English.sml", 7.1563);
// libxml_dom: 6.22 us/byte, 153 Mb/s, 4964 Kb, 37253 allocations
data("speed", "x86", "libxml_dom", "data/English.sml", 6.22073);
data("memory", "x86", "libxml_dom", "data/English.sml", 4.84845);
// libxml_sax: 3.20 us/byte, 298 Mb/s, 2059 Kb, 20 allocations
data("speed", "x86", "libxml_sax", "data/English.sml", 3.19668);
// msxml_dom: 11.08 us/byte, 86 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "msxml_dom", "data/English.sml", 11.0776);
data("memory", "x86", "msxml_dom", "data/English.sml", 2.98438);
// msxml_sax: 12.82 us/byte, 74 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "msxml_sax", "data/English.sml", 12.8248);
// pugixml: 0.79 us/byte, 1212 Mb/s, 1949 Kb, 19 allocations
data("speed", "x86", "pugixml", "data/English.sml", 0.786687);
data("memory", "x86", "pugixml", "data/English.sml", 1.90366);
// pugixml_compact: 1.19 us/byte, 801 Mb/s, 1628 Kb, 16 allocations
data("speed", "x86", "pugixml_compact", "data/English.sml", 1.19038);
data("memory", "x86", "pugixml_compact", "data/English.sml", 1.59004);
// pugxml: 9.17 us/byte, 104 Mb/s, 2650 Kb, 47830 allocations
data("speed", "x86", "pugxml", "data/English.sml", 9.16868);
data("memory", "x86", "pugxml", "data/English.sml", 2.58832);
// qtxml_dom: 28.51 us/byte, 33 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "qtxml_dom", "data/English.sml", 28.5055);
// qtxml_sax: 18.99 us/byte, 50 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "qtxml_sax", "data/English.sml", 18.995);
// rapidxml: 0.96 us/byte, 993 Mb/s, 2203 Kb, 14 allocations
data("speed", "x86", "rapidxml", "data/English.sml", 0.960221);
data("memory", "x86", "rapidxml", "data/English.sml", 2.1523);
// tinyxml: 49.37 us/byte, 19 Mb/s, 3522 Kb, 100500 allocations
data("speed", "x86", "tinyxml", "data/English.sml", 49.3731);
data("memory", "x86", "tinyxml", "data/English.sml", 3.43995);
// tinyxml2: 4.09 us/byte, 233 Mb/s, 2260 Kb, 232 allocations
data("speed", "x86", "tinyxml2", "data/English.sml", 4.08962);
data("memory", "x86", "tinyxml2", "data/English.sml", 2.20755);
// vtdxml: 4.57 us/byte, 209 Mb/s, 1676 Kb, 28 allocations
data("speed", "x86", "vtdxml", "data/English.sml", 4.5676);
data("memory", "x86", "vtdxml", "data/English.sml", 1.63745);
// xerces_dom: 5.42 us/byte, 176 Mb/s, 4646 Kb, 3276 allocations
data("speed", "x86", "xerces_dom", "data/English.sml", 5.41943);
data("memory", "x86", "xerces_dom", "data/English.sml", 4.53774);
// xerces_sax: 3.70 us/byte, 258 Mb/s, 286 Kb, 213 allocations
data("speed", "x86", "xerces_sax", "data/English.sml", 3.70023);
// xmllite: 3.76 us/byte, 254 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "xmllite", "data/English.sml", 3.76142);
// xmlparser: 19.09 us/byte, 50 Mb/s, 2343 Kb, 60082 allocations
data("speed", "x86", "xmlparser", "data/English.sml", 19.0936);
data("memory", "x86", "xmlparser", "data/English.sml", 2.28828);

// data/Russian.sml
// asmxml: 1.07 us/byte, 891 Mb/s, 3930 Kb, 39 allocations
data("speed", "x86", "asmxml", "data/Russian.sml", 1.07033);
data("memory", "x86", "asmxml", "data/Russian.sml", 3.83864);
// cmarkup: 3.93 us/byte, 243 Mb/s, 6385 Kb, 7 allocations
data("speed", "x86", "cmarkup", "data/Russian.sml", 3.9306);
data("memory", "x86", "cmarkup", "data/Russian.sml", 6.23626);
// expat: 3.74 us/byte, 255 Mb/s, 4101 Kb, 22 allocations
data("speed", "x86", "expat", "data/Russian.sml", 3.74142);
// irrxml: 7.49 us/byte, 127 Mb/s, 2567 Kb, 61129 allocations
data("speed", "x86", "irrxml", "data/Russian.sml", 7.48692);
// ixmlparser_dom: 5.51 us/byte, 173 Mb/s, 5220 Kb, 25 allocations
data("speed", "x86", "ixmlparser_dom", "data/Russian.sml", 5.51157);
data("memory", "x86", "ixmlparser_dom", "data/Russian.sml", 5.09803);
// ixmlparser_pull: 4.11 us/byte, 232 Mb/s, 1024 Kb, 2 allocations
data("speed", "x86", "ixmlparser_pull", "data/Russian.sml", 4.11062);
// libxml_dom: 14.19 us/byte, 67 Mb/s, 9402 Kb, 39784 allocations
data("speed", "x86", "libxml_dom", "data/Russian.sml", 14.1854);
data("memory", "x86", "libxml_dom", "data/Russian.sml", 9.18179);
// libxml_sax: 12.05 us/byte, 79 Mb/s, 4108 Kb, 20 allocations
data("speed", "x86", "libxml_sax", "data/Russian.sml", 12.0452);
// msxml_dom: 8.09 us/byte, 118 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "msxml_dom", "data/Russian.sml", 8.0937);
data("memory", "x86", "msxml_dom", "data/Russian.sml", 5.26953);
// msxml_sax: 11.50 us/byte, 83 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "msxml_sax", "data/Russian.sml", 11.5027);
// pugixml: 0.64 us/byte, 1492 Mb/s, 3099 Kb, 18 allocations
data("speed", "x86", "pugixml", "data/Russian.sml", 0.63932);
data("memory", "x86", "pugixml", "data/Russian.sml", 3.02695);
// pugixml_compact: 0.84 us/byte, 1135 Mb/s, 2794 Kb, 15 allocations
data("speed", "x86", "pugixml_compact", "data/Russian.sml", 0.840273);
data("memory", "x86", "pugixml_compact", "data/Russian.sml", 2.72905);
// pugxml: 5.10 us/byte, 187 Mb/s, 3743 Kb, 44471 allocations
data("speed", "x86", "pugxml", "data/Russian.sml", 5.10394);
data("memory", "x86", "pugxml", "data/Russian.sml", 3.65545);
// qtxml_dom: 26.02 us/byte, 37 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "qtxml_dom", "data/Russian.sml", 26.0233);
// qtxml_sax: 11.42 us/byte, 84 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "qtxml_sax", "data/Russian.sml", 11.4198);
// rapidxml: 0.84 us/byte, 1140 Mb/s, 3322 Kb, 13 allocations
data("speed", "x86", "rapidxml", "data/Russian.sml", 0.836676);
data("memory", "x86", "rapidxml", "data/Russian.sml", 3.24442);
// tinyxml: 29.74 us/byte, 32 Mb/s, 5140 Kb, 95136 allocations
data("speed", "x86", "tinyxml", "data/Russian.sml", 29.7372);
data("memory", "x86", "tinyxml", "data/Russian.sml", 5.01982);
// tinyxml2: 2.84 us/byte, 336 Mb/s, 3382 Kb, 216 allocations
data("speed", "x86", "tinyxml2", "data/Russian.sml", 2.8413);
data("memory", "x86", "tinyxml2", "data/Russian.sml", 3.30355);
// vtdxml: 5.80 us/byte, 164 Mb/s, 2859 Kb, 28 allocations
data("speed", "x86", "vtdxml", "data/Russian.sml", 5.80376);
data("memory", "x86", "vtdxml", "data/Russian.sml", 2.79208);
// xerces_dom: 7.38 us/byte, 129 Mb/s, 4879 Kb, 3238 allocations
data("speed", "x86", "xerces_dom", "data/Russian.sml", 7.37917);
data("memory", "x86", "xerces_dom", "data/Russian.sml", 4.76544);
// xerces_sax: 6.45 us/byte, 148 Mb/s, 283 Kb, 212 allocations
data("speed", "x86", "xerces_sax", "data/Russian.sml", 6.45485);
// xmllite: 5.78 us/byte, 165 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "xmllite", "data/Russian.sml", 5.77759);
// xmlparser: 12.19 us/byte, 78 Mb/s, 3457 Kb, 55836 allocations
data("speed", "x86", "xmlparser", "data/Russian.sml", 12.1943);
data("memory", "x86", "xmlparser", "data/Russian.sml", 3.37626);

// data/blog.xml
// asmxml: 1.34 us/byte, 711 Mb/s, 3495 Kb, 444 allocations
data("speed", "x86", "asmxml", "data/blog.xml", 1.34111);
data("memory", "x86", "asmxml", "data/blog.xml", 3.41327);
// cmarkup: 4.76 us/byte, 200 Mb/s, 3260 Kb, 9 allocations
data("speed", "x86", "cmarkup", "data/blog.xml", 4.76071);
data("memory", "x86", "cmarkup", "data/blog.xml", 3.18419);
// expat: 5.12 us/byte, 186 Mb/s, 2056 Kb, 137 allocations
data("speed", "x86", "expat", "data/blog.xml", 5.12219);
// irrxml: 18.96 us/byte, 50 Mb/s, 1323 Kb, 171511 allocations
data("speed", "x86", "irrxml", "data/blog.xml", 18.9631);
// ixmlparser_dom: 9.36 us/byte, 102 Mb/s, 3080 Kb, 16 allocations
data("speed", "x86", "ixmlparser_dom", "data/blog.xml", 9.36376);
data("memory", "x86", "ixmlparser_dom", "data/blog.xml", 3.00803);
// ixmlparser_pull: 7.17 us/byte, 133 Mb/s, 1024 Kb, 2 allocations
data("speed", "x86", "ixmlparser_pull", "data/blog.xml", 7.16735);
// libxml_dom: 9.91 us/byte, 96 Mb/s, 6262 Kb, 79871 allocations
data("speed", "x86", "libxml_dom", "data/blog.xml", 9.9143);
data("memory", "x86", "libxml_dom", "data/blog.xml", 6.11604);
// libxml_sax: 3.92 us/byte, 243 Mb/s, 2068 Kb, 5238 allocations
data("speed", "x86", "libxml_sax", "data/blog.xml", 3.91983);
// msxml_dom: 18.59 us/byte, 51 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "msxml_dom", "data/blog.xml", 18.5868);
data("memory", "x86", "msxml_dom", "data/blog.xml", 3.20313);
// msxml_sax: 12.96 us/byte, 74 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "msxml_sax", "data/blog.xml", 12.9606);
// pugixml: 1.17 us/byte, 815 Mb/s, 2041 Kb, 24 allocations
data("speed", "x86", "pugixml", "data/blog.xml", 1.17063);
data("memory", "x86", "pugixml", "data/blog.xml", 1.99341);
// pugixml_compact: 1.52 us/byte, 627 Mb/s, 1639 Kb, 18 allocations
data("speed", "x86", "pugixml_compact", "data/blog.xml", 1.52089);
data("memory", "x86", "pugixml_compact", "data/blog.xml", 1.60143);
// pugxml: 8.35 us/byte, 114 Mb/s, 2823 Kb, 64805 allocations
data("speed", "x86", "pugxml", "data/blog.xml", 8.34759);
data("memory", "x86", "pugxml", "data/blog.xml", 2.75732);
// qtxml_dom: 42.35 us/byte, 23 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "qtxml_dom", "data/blog.xml", 42.3481);
// qtxml_sax: 29.36 us/byte, 32 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "qtxml_sax", "data/blog.xml", 29.3584);
// rapidxml: 1.29 us/byte, 738 Mb/s, 2391 Kb, 18 allocations
data("speed", "x86", "rapidxml", "data/blog.xml", 1.29176);
data("memory", "x86", "rapidxml", "data/blog.xml", 2.33544);
// tinyxml: 48.74 us/byte, 20 Mb/s, 3954 Kb, 138821 allocations
data("speed", "x86", "tinyxml", "data/blog.xml", 48.7441);
data("memory", "x86", "tinyxml", "data/blog.xml", 3.86146);
// tinyxml2: 5.39 us/byte, 177 Mb/s, 2490 Kb, 309 allocations
data("speed", "x86", "tinyxml2", "data/blog.xml", 5.39233);
data("memory", "x86", "tinyxml2", "data/blog.xml", 2.4325);
// vtdxml: 5.09 us/byte, 187 Mb/s, 1594 Kb, 28 allocations
data("speed", "x86", "vtdxml", "data/blog.xml", 5.09174);
data("memory", "x86", "vtdxml", "data/blog.xml", 1.55684);
// xerces_dom: 11.94 us/byte, 80 Mb/s, 6384 Kb, 3588 allocations
data("speed", "x86", "xerces_dom", "data/blog.xml", 11.938);
data("memory", "x86", "xerces_dom", "data/blog.xml", 6.2345);
// xerces_sax: 8.06 us/byte, 118 Mb/s, 236 Kb, 510 allocations
data("speed", "x86", "xerces_sax", "data/blog.xml", 8.06174);
// xmllite: 3.97 us/byte, 240 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "xmllite", "data/blog.xml", 3.97111);
// xmlparser: 21.48 us/byte, 44 Mb/s, 2460 Kb, 91388 allocations
data("speed", "x86", "xmlparser", "data/blog.xml", 21.4831);
data("memory", "x86", "xmlparser", "data/blog.xml", 2.40239);

// data/cathedral.xml
// asmxml: 1.18 us/byte, 808 Mb/s, 2687 Kb, 56 allocations
data("speed", "x86", "asmxml", "data/cathedral.xml", 1.17964);
data("memory", "x86", "asmxml", "data/cathedral.xml", 2.62406);
// cmarkup: 7.41 us/byte, 129 Mb/s, 2946 Kb, 8 allocations
data("speed", "x86", "cmarkup", "data/cathedral.xml", 7.4087);
data("memory", "x86", "cmarkup", "data/cathedral.xml", 2.87747);
// expat: 17.58 us/byte, 54 Mb/s, 1028 Kb, 23 allocations
data("speed", "x86", "expat", "data/cathedral.xml", 17.5826);
// irrxml: 71.38 us/byte, 13 Mb/s, 906 Kb, 657154 allocations
data("speed", "x86", "irrxml", "data/cathedral.xml", 71.3849);
// ixmlparser_dom: 10.51 us/byte, 91 Mb/s, 4146 Kb, 22 allocations
data("speed", "x86", "ixmlparser_dom", "data/cathedral.xml", 10.5053);
data("memory", "x86", "ixmlparser_dom", "data/cathedral.xml", 4.04919);
// ixmlparser_pull: 5.70 us/byte, 167 Mb/s, 1024 Kb, 2 allocations
data("speed", "x86", "ixmlparser_pull", "data/cathedral.xml", 5.69776);
// libxml_dom: 31.02 us/byte, 31 Mb/s, 9591 Kb, 225750 allocations
data("speed", "x86", "libxml_dom", "data/cathedral.xml", 31.0177);
data("memory", "x86", "libxml_dom", "data/cathedral.xml", 9.36716);
// libxml_sax: 11.40 us/byte, 84 Mb/s, 1035 Kb, 30115 allocations
data("speed", "x86", "libxml_sax", "data/cathedral.xml", 11.3964);
// msxml_dom: 36.61 us/byte, 26 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "msxml_dom", "data/cathedral.xml", 36.6122);
data("memory", "x86", "msxml_dom", "data/cathedral.xml", 3.28516);
// msxml_sax: 13.99 us/byte, 68 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "msxml_sax", "data/cathedral.xml", 13.9852);
// pugixml: 1.38 us/byte, 690 Mb/s, 2125 Kb, 39 allocations
data("speed", "x86", "pugixml", "data/cathedral.xml", 1.38147);
data("memory", "x86", "pugixml", "data/cathedral.xml", 2.07588);
// pugixml_compact: 2.29 us/byte, 417 Mb/s, 1516 Kb, 23 allocations
data("speed", "x86", "pugixml_compact", "data/cathedral.xml", 2.28832);
data("memory", "x86", "pugixml_compact", "data/cathedral.xml", 1.48102);
// pugxml: 17.75 us/byte, 54 Mb/s, 3452 Kb, 136702 allocations
data("speed", "x86", "pugxml", "data/cathedral.xml", 17.7527);
data("memory", "x86", "pugxml", "data/cathedral.xml", 3.37138);
// qtxml_dom: 84.98 us/byte, 11 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "qtxml_dom", "data/cathedral.xml", 84.9849);
// qtxml_sax: 56.77 us/byte, 17 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "qtxml_sax", "data/cathedral.xml", 56.7739);
// rapidxml: 1.61 us/byte, 593 Mb/s, 2634 Kb, 28 allocations
data("speed", "x86", "rapidxml", "data/cathedral.xml", 1.60864);
data("memory", "x86", "rapidxml", "data/cathedral.xml", 2.57301);
// tinyxml: 58.56 us/byte, 16 Mb/s, 5290 Kb, 240788 allocations
data("speed", "x86", "tinyxml", "data/cathedral.xml", 58.5646);
data("memory", "x86", "tinyxml", "data/cathedral.xml", 5.16678);
// tinyxml2: 14.46 us/byte, 66 Mb/s, 3000 Kb, 537 allocations
data("speed", "x86", "tinyxml2", "data/cathedral.xml", 14.458);
data("memory", "x86", "tinyxml2", "data/cathedral.xml", 2.92971);
// vtdxml: 6.48 us/byte, 147 Mb/s, 1797 Kb, 58 allocations
data("speed", "x86", "vtdxml", "data/cathedral.xml", 6.48158);
data("memory", "x86", "vtdxml", "data/cathedral.xml", 1.75542);
// xerces_dom: 31.89 us/byte, 30 Mb/s, 9429 Kb, 249 allocations
data("speed", "x86", "xerces_dom", "data/cathedral.xml", 31.8863);
data("memory", "x86", "xerces_dom", "data/cathedral.xml", 9.20844);
// xerces_sax: 20.26 us/byte, 47 Mb/s, 244 Kb, 221 allocations
data("speed", "x86", "xerces_sax", "data/cathedral.xml", 20.2562);
// xmllite: 6.74 us/byte, 142 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "xmllite", "data/cathedral.xml", 6.73594);
// xmlparser: 25.66 us/byte, 37 Mb/s, 3069 Kb, 170699 allocations
data("speed", "x86", "xmlparser", "data/cathedral.xml", 25.6595);
data("memory", "x86", "xmlparser", "data/cathedral.xml", 2.99764);

// data/employees-big.xml
// asmxml: 1.46 us/byte, 654 Mb/s, 19181 Kb, 285 allocations
data("speed", "x86", "asmxml", "data/employees-big.xml", 1.45737);
data("memory", "x86", "asmxml", "data/employees-big.xml", 18.7317);
// cmarkup: 6.97 us/byte, 137 Mb/s, 28670 Kb, 10 allocations
data("speed", "x86", "cmarkup", "data/employees-big.xml", 6.97111);
data("memory", "x86", "cmarkup", "data/employees-big.xml", 27.9988);
// expat: 12.02 us/byte, 79 Mb/s, 16389 Kb, 23 allocations
data("speed", "x86", "expat", "data/employees-big.xml", 12.0179);
// irrxml: 40.60 us/byte, 23 Mb/s, 10239 Kb, 3963353 allocations
data("speed", "x86", "irrxml", "data/employees-big.xml", 40.5981);
// ixmlparser_dom: 10.93 us/byte, 87 Mb/s, 24976 Kb, 48 allocations
data("speed", "x86", "ixmlparser_dom", "data/employees-big.xml", 10.9273);
data("memory", "x86", "ixmlparser_dom", "data/employees-big.xml", 24.3911);
// ixmlparser_pull: 6.13 us/byte, 156 Mb/s, 1024 Kb, 2 allocations
data("speed", "x86", "ixmlparser_pull", "data/employees-big.xml", 6.1328);
// libxml_dom: 23.69 us/byte, 40 Mb/s, 88863 Kb, 1777509 allocations
data("speed", "x86", "libxml_dom", "data/employees-big.xml", 23.6935);
data("memory", "x86", "libxml_dom", "data/employees-big.xml", 86.781);
// libxml_sax: 7.27 us/byte, 131 Mb/s, 16399 Kb, 84088 allocations
data("speed", "x86", "libxml_sax", "data/employees-big.xml", 7.27346);
// msxml_dom: 31.60 us/byte, 30 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "msxml_dom", "data/employees-big.xml", 31.5962);
data("memory", "x86", "msxml_dom", "data/employees-big.xml", 42.0234);
// msxml_sax: 13.45 us/byte, 71 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "msxml_sax", "data/employees-big.xml", 13.4474);
// pugixml: 1.55 us/byte, 614 Mb/s, 25449 Kb, 475 allocations
data("speed", "x86", "pugixml", "data/employees-big.xml", 1.55399);
data("memory", "x86", "pugixml", "data/employees-big.xml", 24.8532);
// pugixml_compact: 2.47 us/byte, 386 Mb/s, 16587 Kb, 202 allocations
data("speed", "x86", "pugixml_compact", "data/employees-big.xml", 2.46923);
data("memory", "x86", "pugixml_compact", "data/employees-big.xml", 16.199);
// pugxml: 22.26 us/byte, 43 Mb/s, 43171 Kb, 1369156 allocations
data("speed", "x86", "pugxml", "data/employees-big.xml", 22.2553);
data("memory", "x86", "pugxml", "data/employees-big.xml", 42.1592);
// qtxml_dom: 64.42 us/byte, 15 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "qtxml_dom", "data/employees-big.xml", 64.4159);
// qtxml_sax: 37.65 us/byte, 25 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "qtxml_sax", "data/employees-big.xml", 37.6547);
// rapidxml: 1.68 us/byte, 567 Mb/s, 32773 Kb, 353 allocations
data("speed", "x86", "rapidxml", "data/employees-big.xml", 1.68075);
data("memory", "x86", "rapidxml", "data/employees-big.xml", 32.0052);
// tinyxml: 61.96 us/byte, 15 Mb/s, 54796 Kb, 2450061 allocations
data("speed", "x86", "tinyxml", "data/employees-big.xml", 61.9604);
data("memory", "x86", "tinyxml", "data/employees-big.xml", 53.5118);
// tinyxml2: 15.29 us/byte, 62 Mb/s, 34493 Kb, 6110 allocations
data("speed", "x86", "tinyxml2", "data/employees-big.xml", 15.2923);
data("memory", "x86", "tinyxml2", "data/employees-big.xml", 33.6852);
// vtdxml: 5.27 us/byte, 181 Mb/s, 16990 Kb, 603 allocations
data("speed", "x86", "vtdxml", "data/employees-big.xml", 5.27123);
data("memory", "x86", "vtdxml", "data/employees-big.xml", 16.5924);
// xerces_dom: 22.61 us/byte, 42 Mb/s, 80257 Kb, 399 allocations
data("speed", "x86", "xerces_dom", "data/employees-big.xml", 22.609);
data("memory", "x86", "xerces_dom", "data/employees-big.xml", 78.376);
// xerces_sax: 14.18 us/byte, 67 Mb/s, 551 Kb, 233 allocations
data("speed", "x86", "xerces_sax", "data/employees-big.xml", 14.1825);
// xmllite: 7.31 us/byte, 131 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "xmllite", "data/employees-big.xml", 7.30748);
// xmlparser: 29.58 us/byte, 32 Mb/s, 33535 Kb, 1898880 allocations
data("speed", "x86", "xmlparser", "data/employees-big.xml", 29.5754);
data("memory", "x86", "xmlparser", "data/employees-big.xml", 32.7492);

// data/terrover.xml
// asmxml: 1.16 us/byte, 824 Mb/s, 22824 Kb, 40740 allocations
data("speed", "x86", "asmxml", "data/terrover.xml", 1.15764);
data("memory", "x86", "asmxml", "data/terrover.xml", 22.2896);
// cmarkup: 6.09 us/byte, 157 Mb/s, 38444 Kb, 12 allocations
data("speed", "x86", "cmarkup", "data/terrover.xml", 6.09188);
data("memory", "x86", "cmarkup", "data/terrover.xml", 37.5436);
// expat: 14.78 us/byte, 65 Mb/s, 16397 Kb, 132 allocations
data("speed", "x86", "expat", "data/terrover.xml", 14.7776);
// irrxml: 34.38 us/byte, 28 Mb/s, 16164 Kb, 4961420 allocations
data("speed", "x86", "irrxml", "data/terrover.xml", 34.3784);
// ixmlparser_dom: 8.71 us/byte, 109 Mb/s, 26637 Kb, 40 allocations
data("speed", "x86", "ixmlparser_dom", "data/terrover.xml", 8.71072);
data("memory", "x86", "ixmlparser_dom", "data/terrover.xml", 26.0127);
// ixmlparser_pull: 6.28 us/byte, 152 Mb/s, 1024 Kb, 2 allocations
data("speed", "x86", "ixmlparser_pull", "data/terrover.xml", 6.28093);
// libxml_dom: 15.30 us/byte, 62 Mb/s, 89817 Kb, 1652648 allocations
data("speed", "x86", "libxml_dom", "data/terrover.xml", 15.3028);
data("memory", "x86", "libxml_dom", "data/terrover.xml", 87.7123);
// libxml_sax: 6.70 us/byte, 142 Mb/s, 16400 Kb, 230832 allocations
data("speed", "x86", "libxml_sax", "data/terrover.xml", 6.70225);
// msxml_dom: 23.81 us/byte, 40 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "msxml_dom", "data/terrover.xml", 23.8135);
data("memory", "x86", "msxml_dom", "data/terrover.xml", 59.6836);
// msxml_sax: 10.94 us/byte, 87 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "msxml_sax", "data/terrover.xml", 10.9419);
// pugixml: 0.97 us/byte, 984 Mb/s, 25776 Kb, 301 allocations
data("speed", "x86", "pugixml", "data/terrover.xml", 0.969285);
data("memory", "x86", "pugixml", "data/terrover.xml", 25.1727);
// pugixml_compact: 1.44 us/byte, 661 Mb/s, 20927 Kb, 156 allocations
data("speed", "x86", "pugixml_compact", "data/terrover.xml", 1.44366);
data("memory", "x86", "pugixml_compact", "data/terrover.xml", 20.437);
// pugxml: 9.51 us/byte, 100 Mb/s, 35534 Kb, 1031012 allocations
data("speed", "x86", "pugxml", "data/terrover.xml", 9.51339);
data("memory", "x86", "pugxml", "data/terrover.xml", 34.7016);
// qtxml_dom: 69.42 us/byte, 14 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "qtxml_dom", "data/terrover.xml", 69.4201);
// qtxml_sax: 52.90 us/byte, 18 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "qtxml_sax", "data/terrover.xml", 52.8973);
// rapidxml: 1.16 us/byte, 824 Mb/s, 30041 Kb, 218 allocations
data("speed", "x86", "rapidxml", "data/terrover.xml", 1.15689);
data("memory", "x86", "rapidxml", "data/terrover.xml", 29.3376);
// tinyxml: 47.07 us/byte, 20 Mb/s, 52939 Kb, 2131643 allocations
data("speed", "x86", "tinyxml", "data/terrover.xml", 47.0669);
data("memory", "x86", "tinyxml", "data/terrover.xml", 51.699);
// tinyxml2: 8.03 us/byte, 119 Mb/s, 32572 Kb, 4143 allocations
data("speed", "x86", "tinyxml2", "data/terrover.xml", 8.02787);
data("memory", "x86", "tinyxml2", "data/terrover.xml", 31.8095);
// vtdxml: 6.31 us/byte, 151 Mb/s, 21341 Kb, 55 allocations
data("speed", "x86", "vtdxml", "data/terrover.xml", 6.31494);
data("memory", "x86", "vtdxml", "data/terrover.xml", 20.8409);
// xerces_dom: 25.03 us/byte, 38 Mb/s, 83901 Kb, 7388 allocations
data("speed", "x86", "xerces_dom", "data/terrover.xml", 25.0269);
data("memory", "x86", "xerces_dom", "data/terrover.xml", 81.9352);
// xerces_sax: 18.64 us/byte, 51 Mb/s, 236 Kb, 509 allocations
data("speed", "x86", "xerces_sax", "data/terrover.xml", 18.6376);
// xmllite: 4.28 us/byte, 223 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "xmllite", "data/terrover.xml", 4.27523);
// xmlparser: 17.85 us/byte, 53 Mb/s, 32537 Kb, 1234038 allocations
data("speed", "x86", "xmlparser", "data/terrover.xml", 17.8479);
data("memory", "x86", "xmlparser", "data/terrover.xml", 31.7753);

// data/vod.xml
// asmxml: 1.58 us/byte, 602 Mb/s, 5589 Kb, 4723 allocations
data("speed", "x86", "asmxml", "data/vod.xml", 1.58471);
data("memory", "x86", "asmxml", "data/vod.xml", 5.45857);
// cmarkup: 7.86 us/byte, 121 Mb/s, 8216 Kb, 12 allocations
data("speed", "x86", "cmarkup", "data/vod.xml", 7.86188);
data("memory", "x86", "cmarkup", "data/vod.xml", 8.02371);
// expat: 12.47 us/byte, 76 Mb/s, 4102 Kb, 71 allocations
data("speed", "x86", "expat", "data/vod.xml", 12.4731);
// irrxml: 24.91 us/byte, 38 Mb/s, 2134 Kb, 461914 allocations
data("speed", "x86", "irrxml", "data/vod.xml", 24.9054);
// ixmlparser_dom: 13.14 us/byte, 73 Mb/s, 8204 Kb, 22 allocations
data("speed", "x86", "ixmlparser_dom", "data/vod.xml", 13.1401);
data("memory", "x86", "ixmlparser_dom", "data/vod.xml", 8.0126);
// ixmlparser_pull: 7.00 us/byte, 136 Mb/s, 1024 Kb, 2 allocations
data("speed", "x86", "ixmlparser_pull", "data/vod.xml", 7.00117);
// libxml_dom: 28.70 us/byte, 33 Mb/s, 22147 Kb, 443506 allocations
data("speed", "x86", "libxml_dom", "data/vod.xml", 28.6964);
data("memory", "x86", "libxml_dom", "data/vod.xml", 21.6281);
// libxml_sax: 8.64 us/byte, 110 Mb/s, 4108 Kb, 29 allocations
data("speed", "x86", "libxml_sax", "data/vod.xml", 8.6368);
// msxml_dom: 35.77 us/byte, 27 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "msxml_dom", "data/vod.xml", 35.7714);
data("memory", "x86", "msxml_dom", "data/vod.xml", 6.67969);
// msxml_sax: 14.94 us/byte, 64 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "msxml_sax", "data/vod.xml", 14.94);
// pugixml: 1.87 us/byte, 510 Mb/s, 6496 Kb, 137 allocations
data("speed", "x86", "pugixml", "data/vod.xml", 1.86985);
data("memory", "x86", "pugixml", "data/vod.xml", 6.34441);
// pugixml_compact: 3.01 us/byte, 317 Mb/s, 3831 Kb, 61 allocations
data("speed", "x86", "pugixml_compact", "data/vod.xml", 3.01099);
data("memory", "x86", "pugixml_compact", "data/vod.xml", 3.74205);
// pugxml: 17.37 us/byte, 55 Mb/s, 12007 Kb, 382832 allocations
data("speed", "x86", "pugxml", "data/vod.xml", 17.3729);
data("memory", "x86", "pugxml", "data/vod.xml", 11.7256);
// qtxml_dom: 63.81 us/byte, 15 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "qtxml_dom", "data/vod.xml", 63.8088);
// qtxml_sax: 40.07 us/byte, 24 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "qtxml_sax", "data/vod.xml", 40.0686);
// rapidxml: 2.02 us/byte, 471 Mb/s, 8598 Kb, 102 allocations
data("speed", "x86", "rapidxml", "data/vod.xml", 2.02358);
data("memory", "x86", "rapidxml", "data/vod.xml", 8.39672);
// tinyxml: 68.17 us/byte, 14 Mb/s, 14663 Kb, 573268 allocations
data("speed", "x86", "tinyxml", "data/vod.xml", 68.1673);
data("memory", "x86", "tinyxml", "data/vod.xml", 14.3194);
// tinyxml2: 17.94 us/byte, 53 Mb/s, 8970 Kb, 1731 allocations
data("speed", "x86", "tinyxml2", "data/vod.xml", 17.9369);
data("memory", "x86", "tinyxml2", "data/vod.xml", 8.76072);
// vtdxml: 5.35 us/byte, 178 Mb/s, 3589 Kb, 49 allocations
data("speed", "x86", "vtdxml", "data/vod.xml", 5.34968);
data("memory", "x86", "vtdxml", "data/vod.xml", 3.50551);
// xerces_dom: 25.23 us/byte, 38 Mb/s, 20271 Kb, 785 allocations
data("speed", "x86", "xerces_dom", "data/vod.xml", 25.2299);
data("memory", "x86", "xerces_dom", "data/vod.xml", 19.7968);
// xerces_sax: 15.30 us/byte, 62 Mb/s, 215 Kb, 415 allocations
data("speed", "x86", "xerces_sax", "data/vod.xml", 15.3015);
// xmllite: 9.77 us/byte, 98 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "xmllite", "data/vod.xml", 9.77129);
// xmlparser: 33.94 us/byte, 28 Mb/s, 9482 Kb, 482180 allocations
data("speed", "x86", "xmlparser", "data/vod.xml", 33.9399);
data("memory", "x86", "xmlparser", "data/vod.xml", 9.2603);

// data/xmark.xml
// asmxml: 1.32 us/byte, 722 Mb/s, 19066 Kb, 557 allocations
data("speed", "x86", "asmxml", "data/xmark.xml", 1.32075);
data("memory", "x86", "asmxml", "data/xmark.xml", 18.6195);
// cmarkup: 5.46 us/byte, 175 Mb/s, 29338 Kb, 10 allocations
data("speed", "x86", "cmarkup", "data/xmark.xml", 5.45566);
data("memory", "x86", "cmarkup", "data/xmark.xml", 28.6508);
// expat: 6.50 us/byte, 147 Mb/s, 16393 Kb, 118 allocations
data("speed", "x86", "expat", "data/xmark.xml", 6.49653);
// irrxml: 17.86 us/byte, 53 Mb/s, 11604 Kb, 1583123 allocations
data("speed", "x86", "irrxml", "data/xmark.xml", 17.8647);
// ixmlparser_dom: 9.99 us/byte, 95 Mb/s, 22553 Kb, 40 allocations
data("speed", "x86", "ixmlparser_dom", "data/xmark.xml", 9.99265);
data("memory", "x86", "ixmlparser_dom", "data/xmark.xml", 22.025);
// ixmlparser_pull: 6.93 us/byte, 138 Mb/s, 1024 Kb, 2 allocations
data("speed", "x86", "ixmlparser_pull", "data/xmark.xml", 6.9264);
// libxml_dom: 14.23 us/byte, 67 Mb/s, 68957 Kb, 1109077 allocations
data("speed", "x86", "libxml_dom", "data/xmark.xml", 14.2253);
data("memory", "x86", "libxml_dom", "data/xmark.xml", 67.341);
// libxml_sax: 4.30 us/byte, 222 Mb/s, 16399 Kb, 38303 allocations
data("speed", "x86", "libxml_sax", "data/xmark.xml", 4.29769);
// msxml_dom: 21.63 us/byte, 44 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "msxml_dom", "data/xmark.xml", 21.6322);
data("memory", "x86", "msxml_dom", "data/xmark.xml", 29.8398);
// msxml_sax: 12.17 us/byte, 78 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "msxml_sax", "data/xmark.xml", 12.169);
// pugixml: 1.27 us/byte, 752 Mb/s, 21319 Kb, 304 allocations
data("speed", "x86", "pugixml", "data/xmark.xml", 1.26767);
data("memory", "x86", "pugixml", "data/xmark.xml", 20.8203);
// pugixml_compact: 1.75 us/byte, 546 Mb/s, 15540 Kb, 130 allocations
data("speed", "x86", "pugixml_compact", "data/xmark.xml", 1.7451);
data("memory", "x86", "pugixml_compact", "data/xmark.xml", 15.1764);
// pugxml: 10.33 us/byte, 92 Mb/s, 32857 Kb, 866856 allocations
data("speed", "x86", "pugxml", "data/xmark.xml", 10.3271);
data("memory", "x86", "pugxml", "data/xmark.xml", 32.0873);
// qtxml_dom: 46.33 us/byte, 21 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "qtxml_dom", "data/xmark.xml", 46.3349);
// qtxml_sax: 29.13 us/byte, 33 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "qtxml_sax", "data/xmark.xml", 29.1294);
// rapidxml: 1.39 us/byte, 688 Mb/s, 26000 Kb, 226 allocations
data("speed", "x86", "rapidxml", "data/xmark.xml", 1.38694);
data("memory", "x86", "rapidxml", "data/xmark.xml", 25.3913);
// tinyxml: 54.66 us/byte, 17 Mb/s, 42904 Kb, 1596774 allocations
data("speed", "x86", "tinyxml", "data/xmark.xml", 54.6594);
data("memory", "x86", "tinyxml", "data/xmark.xml", 41.899);
// tinyxml2: 8.74 us/byte, 109 Mb/s, 27063 Kb, 3904 allocations
data("speed", "x86", "tinyxml2", "data/xmark.xml", 8.73938);
data("memory", "x86", "tinyxml2", "data/xmark.xml", 26.4292);
// vtdxml: 4.82 us/byte, 198 Mb/s, 14874 Kb, 63 allocations
data("speed", "x86", "vtdxml", "data/xmark.xml", 4.82082);
data("memory", "x86", "vtdxml", "data/xmark.xml", 14.5254);
// xerces_dom: 13.45 us/byte, 71 Mb/s, 62551 Kb, 18608 allocations
data("speed", "x86", "xerces_dom", "data/xmark.xml", 13.4453);
data("memory", "x86", "xerces_dom", "data/xmark.xml", 61.085);
// xerces_sax: 8.19 us/byte, 116 Mb/s, 249 Kb, 598 allocations
data("speed", "x86", "xerces_sax", "data/xmark.xml", 8.18784);
// xmllite: 4.95 us/byte, 193 Mb/s, 0 Kb, 0 allocations
data("speed", "x86", "xmllite", "data/xmark.xml", 4.95237);
// xmlparser: 23.18 us/byte, 41 Mb/s, 27426 Kb, 1169012 allocations
data("speed", "x86", "xmlparser", "data/xmark.xml", 23.1808);
data("memory", "x86", "xmlparser", "data/xmark.xml", 26.7836);

// data/house.dae
// cmarkup: 3.59 us/byte, 265 Mb/s, 15667 Kb, 17 allocations
data("speed", "x64", "cmarkup", "data/house.dae", 3.59204);
data("memory", "x64", "cmarkup", "data/house.dae", 15.3);
// expat: 1.59 us/byte, 600 Mb/s, 8208 Kb, 164 allocations
data("speed", "x64", "expat", "data/house.dae", 1.58998);
// irrxml: 6.84 us/byte, 139 Mb/s, 6670 Kb, 170531 allocations
data("speed", "x64", "irrxml", "data/house.dae", 6.83692);
// ixmlparser_dom: 6.82 us/byte, 140 Mb/s, 8193 Kb, 14 allocations
data("speed", "x64", "ixmlparser_dom", "data/house.dae", 6.81639);
data("memory", "x64", "ixmlparser_dom", "data/house.dae", 8.00192);
// ixmlparser_pull: 5.63 us/byte, 169 Mb/s, 1024 Kb, 2 allocations
data("speed", "x64", "ixmlparser_pull", "data/house.dae", 5.62989);
// libxml_dom: 2.27 us/byte, 420 Mb/s, 18484 Kb, 57098 allocations
data("speed", "x64", "libxml_dom", "data/house.dae", 2.27211);
data("memory", "x64", "libxml_dom", "data/house.dae", 18.0516);
// libxml_sax: 1.50 us/byte, 637 Mb/s, 8207 Kb, 6964 allocations
data("speed", "x64", "libxml_sax", "data/house.dae", 1.49794);
// msxml_dom: 7.51 us/byte, 127 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "msxml_dom", "data/house.dae", 7.50971);
data("memory", "x64", "msxml_dom", "data/house.dae", 41.8008);
// msxml_sax: 7.51 us/byte, 127 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "msxml_sax", "data/house.dae", 7.51238);
// pugixml: 0.55 us/byte, 1730 Mb/s, 7004 Kb, 24 allocations
data("speed", "x64", "pugixml", "data/house.dae", 0.551165);
data("memory", "x64", "pugixml", "data/house.dae", 6.84064);
// pugixml_compact: 0.59 us/byte, 1618 Mb/s, 6474 Kb, 13 allocations
data("speed", "x64", "pugixml_compact", "data/house.dae", 0.589487);
data("memory", "x64", "pugixml_compact", "data/house.dae", 6.32299);
// pugxml: 3.36 us/byte, 284 Mb/s, 7578 Kb, 37428 allocations
data("speed", "x64", "pugxml", "data/house.dae", 3.35624);
data("memory", "x64", "pugxml", "data/house.dae", 7.40063);
// qtxml_dom: 22.88 us/byte, 42 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "qtxml_dom", "data/house.dae", 22.8821);
// qtxml_sax: 18.95 us/byte, 50 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "qtxml_sax", "data/house.dae", 18.9526);
// rapidxml: 0.73 us/byte, 1312 Mb/s, 7290 Kb, 17 allocations
data("speed", "x64", "rapidxml", "data/house.dae", 0.727112);
data("memory", "x64", "rapidxml", "data/house.dae", 7.11984);
// tinyxml: 26.25 us/byte, 36 Mb/s, 11297 Kb, 66621 allocations
data("speed", "x64", "tinyxml", "data/house.dae", 26.2492);
data("memory", "x64", "tinyxml", "data/house.dae", 11.0324);
// tinyxml2: 2.19 us/byte, 435 Mb/s, 7478 Kb, 316 allocations
data("speed", "x64", "tinyxml2", "data/house.dae", 2.191);
data("memory", "x64", "tinyxml2", "data/house.dae", 7.30319);
// vtdxml: 3.77 us/byte, 253 Mb/s, 6539 Kb, 24 allocations
data("speed", "x64", "vtdxml", "data/house.dae", 3.76959);
data("memory", "x64", "vtdxml", "data/house.dae", 6.38622);
// xerces_dom: 2.97 us/byte, 321 Mb/s, 16906 Kb, 5523 allocations
data("speed", "x64", "xerces_dom", "data/house.dae", 2.97496);
data("memory", "x64", "xerces_dom", "data/house.dae", 16.51);
// xerces_sax: 2.28 us/byte, 419 Mb/s, 874 Kb, 739 allocations
data("speed", "x64", "xerces_sax", "data/house.dae", 2.27577);
// xmllite: 1.75 us/byte, 545 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "xmllite", "data/house.dae", 1.7498);
// xmlparser: 12.02 us/byte, 79 Mb/s, 7355 Kb, 48447 allocations
data("speed", "x64", "xmlparser", "data/house.dae", 12.0239);
data("memory", "x64", "xmlparser", "data/house.dae", 7.1833);

// data/English.sml
// cmarkup: 3.92 us/byte, 244 Mb/s, 3430 Kb, 7 allocations
data("speed", "x64", "cmarkup", "data/English.sml", 3.91608);
data("memory", "x64", "cmarkup", "data/English.sml", 3.34978);
// expat: 2.05 us/byte, 464 Mb/s, 2054 Kb, 22 allocations
data("speed", "x64", "expat", "data/English.sml", 2.05379);
// irrxml: 8.55 us/byte, 112 Mb/s, 1379 Kb, 64680 allocations
data("speed", "x64", "irrxml", "data/English.sml", 8.55218);
// ixmlparser_dom: 8.42 us/byte, 113 Mb/s, 4246 Kb, 24 allocations
data("speed", "x64", "ixmlparser_dom", "data/English.sml", 8.41579);
data("memory", "x64", "ixmlparser_dom", "data/English.sml", 4.1472);
// ixmlparser_pull: 6.57 us/byte, 145 Mb/s, 1024 Kb, 2 allocations
data("speed", "x64", "ixmlparser_pull", "data/English.sml", 6.56993);
// libxml_dom: 6.08 us/byte, 157 Mb/s, 6301 Kb, 37253 allocations
data("speed", "x64", "libxml_dom", "data/English.sml", 6.08039);
data("memory", "x64", "libxml_dom", "data/English.sml", 6.15383);
// libxml_sax: 3.30 us/byte, 289 Mb/s, 2062 Kb, 20 allocations
data("speed", "x64", "libxml_sax", "data/English.sml", 3.29623);
// msxml_dom: 11.69 us/byte, 82 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "msxml_dom", "data/English.sml", 11.691);
data("memory", "x64", "msxml_dom", "data/English.sml", 3.22656);
// msxml_sax: 11.26 us/byte, 85 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "msxml_sax", "data/English.sml", 11.2563);
// pugixml: 0.75 us/byte, 1278 Mb/s, 2495 Kb, 36 allocations
data("speed", "x64", "pugixml", "data/English.sml", 0.746196);
data("memory", "x64", "pugixml", "data/English.sml", 2.43734);
// pugixml_compact: 1.08 us/byte, 886 Mb/s, 1660 Kb, 16 allocations
data("speed", "x64", "pugixml_compact", "data/English.sml", 1.07591);
data("memory", "x64", "pugixml_compact", "data/English.sml", 1.62154);
// pugxml: 10.26 us/byte, 93 Mb/s, 3511 Kb, 47830 allocations
data("speed", "x64", "pugxml", "data/English.sml", 10.2619);
data("memory", "x64", "pugxml", "data/English.sml", 3.4295);
// qtxml_dom: 28.54 us/byte, 33 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "qtxml_dom", "data/English.sml", 28.5366);
// qtxml_sax: 19.65 us/byte, 49 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "qtxml_sax", "data/English.sml", 19.6517);
// rapidxml: 0.96 us/byte, 996 Mb/s, 3036 Kb, 27 allocations
data("speed", "x64", "rapidxml", "data/English.sml", 0.957481);
data("memory", "x64", "rapidxml", "data/English.sml", 2.96553);
// tinyxml: 35.00 us/byte, 27 Mb/s, 4934 Kb, 100500 allocations
data("speed", "x64", "tinyxml", "data/English.sml", 35.0027);
data("memory", "x64", "tinyxml", "data/English.sml", 4.8186);
// tinyxml2: 3.91 us/byte, 244 Mb/s, 3142 Kb, 456 allocations
data("speed", "x64", "tinyxml2", "data/English.sml", 3.91243);
data("memory", "x64", "tinyxml2", "data/English.sml", 3.06911);
// vtdxml: 4.09 us/byte, 233 Mb/s, 1677 Kb, 28 allocations
data("speed", "x64", "vtdxml", "data/English.sml", 4.08535);
data("memory", "x64", "vtdxml", "data/English.sml", 1.63785);
// xerces_dom: 4.70 us/byte, 203 Mb/s, 5748 Kb, 3278 allocations
data("speed", "x64", "xerces_dom", "data/English.sml", 4.70064);
data("memory", "x64", "xerces_dom", "data/English.sml", 5.61424);
// xerces_sax: 3.31 us/byte, 288 Mb/s, 371 Kb, 213 allocations
data("speed", "x64", "xerces_sax", "data/English.sml", 3.30993);
// xmllite: 3.03 us/byte, 315 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "xmllite", "data/English.sml", 3.02984);
// xmlparser: 17.51 us/byte, 54 Mb/s, 3136 Kb, 60082 allocations
data("speed", "x64", "xmlparser", "data/English.sml", 17.5138);
data("memory", "x64", "xmlparser", "data/English.sml", 3.06335);

// data/Russian.sml
// cmarkup: 3.77 us/byte, 253 Mb/s, 6386 Kb, 7 allocations
data("speed", "x64", "cmarkup", "data/Russian.sml", 3.7666);
data("memory", "x64", "cmarkup", "data/Russian.sml", 6.2364);
// expat: 3.78 us/byte, 252 Mb/s, 4102 Kb, 22 allocations
data("speed", "x64", "expat", "data/Russian.sml", 3.77886);
// irrxml: 6.77 us/byte, 141 Mb/s, 2567 Kb, 61129 allocations
data("speed", "x64", "irrxml", "data/Russian.sml", 6.7732);
// ixmlparser_dom: 6.22 us/byte, 153 Mb/s, 5270 Kb, 25 allocations
data("speed", "x64", "ixmlparser_dom", "data/Russian.sml", 6.22316);
data("memory", "x64", "ixmlparser_dom", "data/Russian.sml", 5.14722);
// ixmlparser_pull: 4.69 us/byte, 203 Mb/s, 1024 Kb, 2 allocations
data("speed", "x64", "ixmlparser_pull", "data/Russian.sml", 4.69288);
// libxml_dom: 14.09 us/byte, 68 Mb/s, 10646 Kb, 39784 allocations
data("speed", "x64", "libxml_dom", "data/Russian.sml", 14.0866);
data("memory", "x64", "libxml_dom", "data/Russian.sml", 10.3973);
// libxml_sax: 12.17 us/byte, 78 Mb/s, 4110 Kb, 20 allocations
data("speed", "x64", "libxml_sax", "data/Russian.sml", 12.1657);
// msxml_dom: 8.55 us/byte, 112 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "msxml_dom", "data/Russian.sml", 8.55038);
data("memory", "x64", "msxml_dom", "data/Russian.sml", 5.51172);
// msxml_sax: 9.95 us/byte, 96 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "msxml_sax", "data/Russian.sml", 9.95296);
// pugixml: 0.62 us/byte, 1534 Mb/s, 3613 Kb, 34 allocations
data("speed", "x64", "pugixml", "data/Russian.sml", 0.621662);
data("memory", "x64", "pugixml", "data/Russian.sml", 3.52925);
// pugixml_compact: 0.78 us/byte, 1221 Mb/s, 2810 Kb, 15 allocations
data("speed", "x64", "pugixml_compact", "data/Russian.sml", 0.781083);
data("memory", "x64", "pugixml_compact", "data/Russian.sml", 2.74493);
// pugxml: 5.45 us/byte, 175 Mb/s, 4544 Kb, 44471 allocations
data("speed", "x64", "pugxml", "data/Russian.sml", 5.44551);
data("memory", "x64", "pugxml", "data/Russian.sml", 4.43773);
// qtxml_dom: 24.09 us/byte, 40 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "qtxml_dom", "data/Russian.sml", 24.0937);
// qtxml_sax: 11.71 us/byte, 81 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "qtxml_sax", "data/Russian.sml", 11.712);
// rapidxml: 0.84 us/byte, 1135 Mb/s, 4090 Kb, 25 allocations
data("speed", "x64", "rapidxml", "data/Russian.sml", 0.840437);
data("memory", "x64", "rapidxml", "data/Russian.sml", 3.99509);
// tinyxml: 21.06 us/byte, 45 Mb/s, 6452 Kb, 95136 allocations
data("speed", "x64", "tinyxml", "data/Russian.sml", 21.0552);
data("memory", "x64", "tinyxml", "data/Russian.sml", 6.30153);
// tinyxml2: 2.74 us/byte, 348 Mb/s, 4200 Kb, 424 allocations
data("speed", "x64", "tinyxml2", "data/Russian.sml", 2.73976);
data("memory", "x64", "tinyxml2", "data/Russian.sml", 4.1022);
// vtdxml: 5.10 us/byte, 187 Mb/s, 2859 Kb, 28 allocations
data("speed", "x64", "vtdxml", "data/Russian.sml", 5.10279);
data("memory", "x64", "vtdxml", "data/Russian.sml", 2.79249);
// xerces_dom: 6.15 us/byte, 155 Mb/s, 5981 Kb, 3240 allocations
data("speed", "x64", "xerces_dom", "data/Russian.sml", 6.1468);
data("memory", "x64", "xerces_dom", "data/Russian.sml", 5.84166);
// xerces_sax: 5.38 us/byte, 177 Mb/s, 368 Kb, 212 allocations
data("speed", "x64", "xerces_sax", "data/Russian.sml", 5.37912);
// xmllite: 4.87 us/byte, 196 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "xmllite", "data/Russian.sml", 4.86669);
// xmlparser: 10.79 us/byte, 88 Mb/s, 4195 Kb, 55836 allocations
data("speed", "x64", "xmlparser", "data/Russian.sml", 10.7937);
data("memory", "x64", "xmlparser", "data/Russian.sml", 4.09725);

// data/blog.xml
// cmarkup: 4.49 us/byte, 213 Mb/s, 3261 Kb, 9 allocations
data("speed", "x64", "cmarkup", "data/blog.xml", 4.48576);
data("memory", "x64", "cmarkup", "data/blog.xml", 3.18461);
// expat: 4.42 us/byte, 216 Mb/s, 2061 Kb, 137 allocations
data("speed", "x64", "expat", "data/blog.xml", 4.41622);
// irrxml: 17.12 us/byte, 56 Mb/s, 1323 Kb, 171511 allocations
data("speed", "x64", "irrxml", "data/blog.xml", 17.123);
// ixmlparser_dom: 8.79 us/byte, 108 Mb/s, 4109 Kb, 17 allocations
data("speed", "x64", "ixmlparser_dom", "data/blog.xml", 8.7911);
data("memory", "x64", "ixmlparser_dom", "data/blog.xml", 4.01302);
// ixmlparser_pull: 6.55 us/byte, 146 Mb/s, 1024 Kb, 2 allocations
data("speed", "x64", "ixmlparser_pull", "data/blog.xml", 6.54694);
// libxml_dom: 9.29 us/byte, 103 Mb/s, 9096 Kb, 79871 allocations
data("speed", "x64", "libxml_dom", "data/blog.xml", 9.2907);
data("memory", "x64", "libxml_dom", "data/blog.xml", 8.88346);
// libxml_sax: 3.76 us/byte, 254 Mb/s, 2080 Kb, 5238 allocations
data("speed", "x64", "libxml_sax", "data/blog.xml", 3.75992);
// msxml_dom: 18.58 us/byte, 51 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "msxml_dom", "data/blog.xml", 18.5782);
data("memory", "x64", "msxml_dom", "data/blog.xml", 3.96484);
// msxml_sax: 11.11 us/byte, 86 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "msxml_sax", "data/blog.xml", 11.1115);
// pugixml: 1.11 us/byte, 862 Mb/s, 2780 Kb, 47 allocations
data("speed", "x64", "pugixml", "data/blog.xml", 1.1059);
data("memory", "x64", "pugixml", "data/blog.xml", 2.71542);
// pugixml_compact: 1.42 us/byte, 673 Mb/s, 1656 Kb, 18 allocations
data("speed", "x64", "pugixml_compact", "data/blog.xml", 1.41802);
data("memory", "x64", "pugixml_compact", "data/blog.xml", 1.61741);
// pugxml: 7.68 us/byte, 124 Mb/s, 3877 Kb, 64805 allocations
data("speed", "x64", "pugxml", "data/blog.xml", 7.68328);
data("memory", "x64", "pugxml", "data/blog.xml", 3.78656);
// qtxml_dom: 37.61 us/byte, 25 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "qtxml_dom", "data/blog.xml", 37.613);
// qtxml_sax: 24.23 us/byte, 39 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "qtxml_sax", "data/blog.xml", 24.2314);
// rapidxml: 1.30 us/byte, 734 Mb/s, 3480 Kb, 35 allocations
data("speed", "x64", "rapidxml", "data/blog.xml", 1.29977);
data("memory", "x64", "rapidxml", "data/blog.xml", 3.39888);
// tinyxml: 35.77 us/byte, 27 Mb/s, 5925 Kb, 138821 allocations
data("speed", "x64", "tinyxml", "data/blog.xml", 35.772);
data("memory", "x64", "tinyxml", "data/blog.xml", 5.78624);
// tinyxml2: 5.32 us/byte, 179 Mb/s, 3676 Kb, 611 allocations
data("speed", "x64", "tinyxml2", "data/blog.xml", 5.31991);
data("memory", "x64", "tinyxml2", "data/blog.xml", 3.59007);
// vtdxml: 4.59 us/byte, 208 Mb/s, 1594 Kb, 28 allocations
data("speed", "x64", "vtdxml", "data/blog.xml", 4.59375);
data("memory", "x64", "vtdxml", "data/blog.xml", 1.55725);
// xerces_dom: 9.80 us/byte, 97 Mb/s, 8481 Kb, 3592 allocations
data("speed", "x64", "xerces_dom", "data/blog.xml", 9.79862);
data("memory", "x64", "xerces_dom", "data/blog.xml", 8.28272);
// xerces_sax: 6.84 us/byte, 140 Mb/s, 260 Kb, 510 allocations
data("speed", "x64", "xerces_sax", "data/blog.xml", 6.83503);
// xmllite: 3.50 us/byte, 272 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "xmllite", "data/blog.xml", 3.50292);
// xmlparser: 19.31 us/byte, 49 Mb/s, 3463 Kb, 91388 allocations
data("speed", "x64", "xmlparser", "data/blog.xml", 19.3079);
data("memory", "x64", "xmlparser", "data/blog.xml", 3.38209);

// data/cathedral.xml
// cmarkup: 6.73 us/byte, 142 Mb/s, 2946 Kb, 8 allocations
data("speed", "x64", "cmarkup", "data/cathedral.xml", 6.73133);
data("memory", "x64", "cmarkup", "data/cathedral.xml", 2.87762);
// expat: 14.65 us/byte, 65 Mb/s, 1030 Kb, 23 allocations
data("speed", "x64", "expat", "data/cathedral.xml", 14.6487);
// irrxml: 63.27 us/byte, 15 Mb/s, 907 Kb, 657154 allocations
data("speed", "x64", "irrxml", "data/cathedral.xml", 63.2652);
// ixmlparser_dom: 10.23 us/byte, 93 Mb/s, 5195 Kb, 23 allocations
data("speed", "x64", "ixmlparser_dom", "data/cathedral.xml", 10.2288);
data("memory", "x64", "ixmlparser_dom", "data/cathedral.xml", 5.07397);
// ixmlparser_pull: 5.39 us/byte, 177 Mb/s, 1024 Kb, 2 allocations
data("speed", "x64", "ixmlparser_pull", "data/cathedral.xml", 5.39456);
// libxml_dom: 28.91 us/byte, 33 Mb/s, 17432 Kb, 225750 allocations
data("speed", "x64", "libxml_dom", "data/cathedral.xml", 28.9063);
data("memory", "x64", "libxml_dom", "data/cathedral.xml", 17.0238);
// libxml_sax: 10.74 us/byte, 89 Mb/s, 1037 Kb, 30115 allocations
data("speed", "x64", "libxml_sax", "data/cathedral.xml", 10.7389);
// msxml_dom: 37.31 us/byte, 26 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "msxml_dom", "data/cathedral.xml", 37.3135);
data("memory", "x64", "msxml_dom", "data/cathedral.xml", 4.82031);
// msxml_sax: 11.32 us/byte, 84 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "msxml_sax", "data/cathedral.xml", 11.3241);
// pugixml: 1.21 us/byte, 787 Mb/s, 3379 Kb, 78 allocations
data("speed", "x64", "pugixml", "data/cathedral.xml", 1.21144);
data("memory", "x64", "pugixml", "data/cathedral.xml", 3.30012);
// pugixml_compact: 1.93 us/byte, 493 Mb/s, 1518 Kb, 23 allocations
data("speed", "x64", "pugixml_compact", "data/cathedral.xml", 1.93258);
data("memory", "x64", "pugixml_compact", "data/cathedral.xml", 1.48266);
// pugxml: 16.76 us/byte, 57 Mb/s, 5528 Kb, 136702 allocations
data("speed", "x64", "pugxml", "data/cathedral.xml", 16.7624);
data("memory", "x64", "pugxml", "data/cathedral.xml", 5.39868);
// qtxml_dom: 72.13 us/byte, 13 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "qtxml_dom", "data/cathedral.xml", 72.1323);
// qtxml_sax: 42.88 us/byte, 22 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "qtxml_sax", "data/cathedral.xml", 42.88);
// rapidxml: 1.58 us/byte, 603 Mb/s, 4428 Kb, 56 allocations
data("speed", "x64", "rapidxml", "data/cathedral.xml", 1.58238);
data("memory", "x64", "rapidxml", "data/cathedral.xml", 4.32455);
// tinyxml: 47.95 us/byte, 20 Mb/s, 9327 Kb, 240788 allocations
data("speed", "x64", "tinyxml", "data/cathedral.xml", 47.9464);
data("memory", "x64", "tinyxml", "data/cathedral.xml", 9.10884);
// tinyxml2: 13.97 us/byte, 68 Mb/s, 5096 Kb, 1067 allocations
data("speed", "x64", "tinyxml2", "data/cathedral.xml", 13.9682);
data("memory", "x64", "tinyxml2", "data/cathedral.xml", 4.97752);
// vtdxml: 5.57 us/byte, 171 Mb/s, 1798 Kb, 58 allocations
data("speed", "x64", "vtdxml", "data/cathedral.xml", 5.56874);
data("memory", "x64", "vtdxml", "data/cathedral.xml", 1.75605);
// xerces_dom: 27.11 us/byte, 35 Mb/s, 14584 Kb, 259 allocations
data("speed", "x64", "xerces_dom", "data/cathedral.xml", 27.1147);
data("memory", "x64", "xerces_dom", "data/cathedral.xml", 14.243);
// xerces_sax: 16.56 us/byte, 58 Mb/s, 294 Kb, 221 allocations
data("speed", "x64", "xerces_sax", "data/cathedral.xml", 16.556);
// xmllite: 6.06 us/byte, 157 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "xmllite", "data/cathedral.xml", 6.0581);
// xmlparser: 21.79 us/byte, 44 Mb/s, 5167 Kb, 170699 allocations
data("speed", "x64", "xmlparser", "data/cathedral.xml", 21.7893);
data("memory", "x64", "xmlparser", "data/cathedral.xml", 5.04624);

// data/employees-big.xml
// cmarkup: 6.55 us/byte, 145 Mb/s, 28670 Kb, 10 allocations
data("speed", "x64", "cmarkup", "data/employees-big.xml", 6.55481);
data("memory", "x64", "cmarkup", "data/employees-big.xml", 27.999);
// expat: 10.58 us/byte, 90 Mb/s, 16390 Kb, 23 allocations
data("speed", "x64", "expat", "data/employees-big.xml", 10.5787);
// irrxml: 36.29 us/byte, 26 Mb/s, 10240 Kb, 3963353 allocations
data("speed", "x64", "irrxml", "data/employees-big.xml", 36.2886);
// ixmlparser_dom: 10.14 us/byte, 94 Mb/s, 36441 Kb, 59 allocations
data("speed", "x64", "ixmlparser_dom", "data/employees-big.xml", 10.1422);
data("memory", "x64", "ixmlparser_dom", "data/employees-big.xml", 35.5871);
// ixmlparser_pull: 5.69 us/byte, 168 Mb/s, 1024 Kb, 2 allocations
data("speed", "x64", "ixmlparser_pull", "data/employees-big.xml", 5.69255);
// libxml_dom: 22.50 us/byte, 42 Mb/s, 153842 Kb, 1777509 allocations
data("speed", "x64", "libxml_dom", "data/employees-big.xml", 22.5014);
data("memory", "x64", "libxml_dom", "data/employees-big.xml", 150.237);
// libxml_sax: 6.86 us/byte, 139 Mb/s, 16401 Kb, 84088 allocations
data("speed", "x64", "libxml_sax", "data/employees-big.xml", 6.8614);
// msxml_dom: 40.88 us/byte, 23 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "msxml_dom", "data/employees-big.xml", 40.878);
data("memory", "x64", "msxml_dom", "data/employees-big.xml", 56.0313);
// msxml_sax: 11.10 us/byte, 86 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "msxml_sax", "data/employees-big.xml", 11.1017);
// pugixml: 1.45 us/byte, 659 Mb/s, 40718 Kb, 950 allocations
data("speed", "x64", "pugixml", "data/employees-big.xml", 1.44607);
data("memory", "x64", "pugixml", "data/employees-big.xml", 39.764);
// pugixml_compact: 2.20 us/byte, 433 Mb/s, 16850 Kb, 202 allocations
data("speed", "x64", "pugixml_compact", "data/employees-big.xml", 2.20184);
data("memory", "x64", "pugixml_compact", "data/employees-big.xml", 16.4555);
// pugxml: 28.53 us/byte, 33 Mb/s, 65970 Kb, 1369156 allocations
data("speed", "x64", "pugxml", "data/employees-big.xml", 28.529);
data("memory", "x64", "pugxml", "data/employees-big.xml", 64.4239);
// qtxml_dom: 55.62 us/byte, 17 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "qtxml_dom", "data/employees-big.xml", 55.6155);
// qtxml_sax: 31.16 us/byte, 31 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "qtxml_sax", "data/employees-big.xml", 31.1586);
// rapidxml: 1.62 us/byte, 590 Mb/s, 55385 Kb, 706 allocations
data("speed", "x64", "rapidxml", "data/employees-big.xml", 1.61721);
data("memory", "x64", "rapidxml", "data/employees-big.xml", 54.0872);
// tinyxml: 49.18 us/byte, 19 Mb/s, 95377 Kb, 2450061 allocations
data("speed", "x64", "tinyxml", "data/employees-big.xml", 49.1834);
data("memory", "x64", "tinyxml", "data/employees-big.xml", 93.1417);
// tinyxml2: 15.17 us/byte, 63 Mb/s, 58921 Kb, 12266 allocations
data("speed", "x64", "tinyxml2", "data/employees-big.xml", 15.1702);
data("memory", "x64", "tinyxml2", "data/employees-big.xml", 57.5408);
// vtdxml: 4.84 us/byte, 197 Mb/s, 16995 Kb, 603 allocations
data("speed", "x64", "vtdxml", "data/employees-big.xml", 4.8429);
data("memory", "x64", "vtdxml", "data/employees-big.xml", 16.5972);
// xerces_dom: 20.73 us/byte, 46 Mb/s, 125521 Kb, 487 allocations
data("speed", "x64", "xerces_dom", "data/employees-big.xml", 20.7309);
data("memory", "x64", "xerces_dom", "data/employees-big.xml", 122.579);
// xerces_sax: 12.01 us/byte, 79 Mb/s, 907 Kb, 233 allocations
data("speed", "x64", "xerces_sax", "data/employees-big.xml", 12.0144);
// xmllite: 6.60 us/byte, 144 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "xmllite", "data/employees-big.xml", 6.60412);
// xmlparser: 26.44 us/byte, 36 Mb/s, 55210 Kb, 1898880 allocations
data("speed", "x64", "xmlparser", "data/employees-big.xml", 26.4373);
data("memory", "x64", "xmlparser", "data/employees-big.xml", 53.9167);

// data/terrover.xml
// cmarkup: 5.93 us/byte, 161 Mb/s, 38444 Kb, 12 allocations
data("speed", "x64", "cmarkup", "data/terrover.xml", 5.92571);
data("memory", "x64", "cmarkup", "data/terrover.xml", 37.5439);
// expat: 12.27 us/byte, 78 Mb/s, 16402 Kb, 132 allocations
data("speed", "x64", "expat", "data/terrover.xml", 12.2655);
// irrxml: 29.81 us/byte, 32 Mb/s, 16165 Kb, 4961420 allocations
data("speed", "x64", "irrxml", "data/terrover.xml", 29.81);
// ixmlparser_dom: 8.35 us/byte, 114 Mb/s, 35859 Kb, 49 allocations
data("speed", "x64", "ixmlparser_dom", "data/terrover.xml", 8.35185);
data("memory", "x64", "ixmlparser_dom", "data/terrover.xml", 35.0195);
// ixmlparser_pull: 5.71 us/byte, 167 Mb/s, 1024 Kb, 2 allocations
data("speed", "x64", "ixmlparser_pull", "data/terrover.xml", 5.70599);
// libxml_dom: 14.82 us/byte, 64 Mb/s, 148443 Kb, 1652648 allocations
data("speed", "x64", "libxml_dom", "data/terrover.xml", 14.8209);
data("memory", "x64", "libxml_dom", "data/terrover.xml", 144.964);
// libxml_sax: 6.01 us/byte, 159 Mb/s, 16403 Kb, 230832 allocations
data("speed", "x64", "libxml_sax", "data/terrover.xml", 6.01493);
// msxml_dom: 36.16 us/byte, 26 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "msxml_dom", "data/terrover.xml", 36.1594);
data("memory", "x64", "msxml_dom", "data/terrover.xml", 71.9453);
// msxml_sax: 9.31 us/byte, 102 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "msxml_sax", "data/terrover.xml", 9.30699);
// pugixml: 0.93 us/byte, 1021 Mb/s, 35420 Kb, 601 allocations
data("speed", "x64", "pugixml", "data/terrover.xml", 0.933859);
data("memory", "x64", "pugixml", "data/terrover.xml", 34.59);
// pugixml_compact: 1.32 us/byte, 723 Mb/s, 21060 Kb, 156 allocations
data("speed", "x64", "pugixml_compact", "data/terrover.xml", 1.31933);
data("memory", "x64", "pugixml_compact", "data/terrover.xml", 20.567);
// pugxml: 8.82 us/byte, 108 Mb/s, 51112 Kb, 1031012 allocations
data("speed", "x64", "pugxml", "data/terrover.xml", 8.81888);
data("memory", "x64", "pugxml", "data/terrover.xml", 49.915);
// qtxml_dom: 47.96 us/byte, 20 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "qtxml_dom", "data/terrover.xml", 47.9648);
// qtxml_sax: 31.39 us/byte, 30 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "qtxml_sax", "data/terrover.xml", 31.3913);
// rapidxml: 1.19 us/byte, 804 Mb/s, 44006 Kb, 436 allocations
data("speed", "x64", "rapidxml", "data/terrover.xml", 1.18624);
data("memory", "x64", "rapidxml", "data/terrover.xml", 42.9746);
// tinyxml: 34.59 us/byte, 28 Mb/s, 84489 Kb, 2131643 allocations
data("speed", "x64", "tinyxml", "data/terrover.xml", 34.5912);
data("memory", "x64", "tinyxml", "data/terrover.xml", 82.5089);
// tinyxml2: 8.19 us/byte, 116 Mb/s, 49103 Kb, 8306 allocations
data("speed", "x64", "tinyxml2", "data/terrover.xml", 8.18607);
data("memory", "x64", "tinyxml2", "data/terrover.xml", 47.9522);
// vtdxml: 5.60 us/byte, 170 Mb/s, 21341 Kb, 55 allocations
data("speed", "x64", "vtdxml", "data/terrover.xml", 5.60388);
data("memory", "x64", "vtdxml", "data/terrover.xml", 20.8415);
// xerces_dom: 20.94 us/byte, 46 Mb/s, 124431 Kb, 7467 allocations
data("speed", "x64", "xerces_dom", "data/terrover.xml", 20.9439);
data("memory", "x64", "xerces_dom", "data/terrover.xml", 121.515);
// xerces_sax: 15.16 us/byte, 63 Mb/s, 261 Kb, 509 allocations
data("speed", "x64", "xerces_sax", "data/terrover.xml", 15.1606);
// xmllite: 3.73 us/byte, 256 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "xmllite", "data/terrover.xml", 3.72788);
// xmlparser: 16.60 us/byte, 57 Mb/s, 48579 Kb, 1234038 allocations
data("speed", "x64", "xmlparser", "data/terrover.xml", 16.5964);
data("memory", "x64", "xmlparser", "data/terrover.xml", 47.4411);

// data/vod.xml
// cmarkup: 7.29 us/byte, 131 Mb/s, 8216 Kb, 12 allocations
data("speed", "x64", "cmarkup", "data/vod.xml", 7.28572);
data("memory", "x64", "cmarkup", "data/vod.xml", 8.02387);
// expat: 11.65 us/byte, 82 Mb/s, 4105 Kb, 71 allocations
data("speed", "x64", "expat", "data/vod.xml", 11.6505);
// irrxml: 21.12 us/byte, 45 Mb/s, 2134 Kb, 461914 allocations
data("speed", "x64", "irrxml", "data/vod.xml", 21.1163);
// ixmlparser_dom: 12.16 us/byte, 78 Mb/s, 11283 Kb, 25 allocations
data("speed", "x64", "ixmlparser_dom", "data/vod.xml", 12.163);
data("memory", "x64", "ixmlparser_dom", "data/vod.xml", 11.0191);
// ixmlparser_pull: 6.47 us/byte, 147 Mb/s, 1024 Kb, 2 allocations
data("speed", "x64", "ixmlparser_pull", "data/vod.xml", 6.47417);
// libxml_dom: 26.63 us/byte, 36 Mb/s, 38602 Kb, 443506 allocations
data("speed", "x64", "libxml_dom", "data/vod.xml", 26.6303);
data("memory", "x64", "libxml_dom", "data/vod.xml", 37.6982);
// libxml_sax: 8.14 us/byte, 117 Mb/s, 4110 Kb, 29 allocations
data("speed", "x64", "libxml_sax", "data/vod.xml", 8.14192);
// msxml_dom: 32.46 us/byte, 29 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "msxml_dom", "data/vod.xml", 32.4633);
data("memory", "x64", "msxml_dom", "data/vod.xml", 9.31641);
// msxml_sax: 12.01 us/byte, 79 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "msxml_sax", "data/vod.xml", 12.0119);
// pugixml: 1.94 us/byte, 490 Mb/s, 10836 Kb, 272 allocations
data("speed", "x64", "pugixml", "data/vod.xml", 1.94486);
data("memory", "x64", "pugixml", "data/vod.xml", 10.5823);
// pugixml_compact: 2.70 us/byte, 353 Mb/s, 3897 Kb, 61 allocations
data("speed", "x64", "pugixml_compact", "data/vod.xml", 2.69863);
data("memory", "x64", "pugixml_compact", "data/vod.xml", 3.80631);
// pugxml: 15.58 us/byte, 61 Mb/s, 18640 Kb, 382832 allocations
data("speed", "x64", "pugxml", "data/vod.xml", 15.5775);
data("memory", "x64", "pugxml", "data/vod.xml", 18.2032);
// qtxml_dom: 58.16 us/byte, 16 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "qtxml_dom", "data/vod.xml", 58.1603);
// qtxml_sax: 34.24 us/byte, 28 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "qtxml_sax", "data/vod.xml", 34.2382);
// rapidxml: 2.18 us/byte, 437 Mb/s, 15067 Kb, 203 allocations
data("speed", "x64", "rapidxml", "data/vod.xml", 2.18319);
data("memory", "x64", "rapidxml", "data/vod.xml", 14.7148);
// tinyxml: 54.81 us/byte, 17 Mb/s, 25801 Kb, 573268 allocations
data("speed", "x64", "tinyxml", "data/vod.xml", 54.8148);
data("memory", "x64", "tinyxml", "data/vod.xml", 25.1972);
// tinyxml2: 17.93 us/byte, 53 Mb/s, 15845 Kb, 3463 allocations
data("speed", "x64", "tinyxml2", "data/vod.xml", 17.9279);
data("memory", "x64", "tinyxml2", "data/vod.xml", 15.4736);
// vtdxml: 5.13 us/byte, 186 Mb/s, 3590 Kb, 49 allocations
data("speed", "x64", "vtdxml", "data/vod.xml", 5.12975);
data("memory", "x64", "vtdxml", "data/vod.xml", 3.50608);
// xerces_dom: 20.13 us/byte, 47 Mb/s, 32588 Kb, 809 allocations
data("speed", "x64", "xerces_dom", "data/vod.xml", 20.1297);
data("memory", "x64", "xerces_dom", "data/vod.xml", 31.825);
// xerces_sax: 13.61 us/byte, 70 Mb/s, 243 Kb, 415 allocations
data("speed", "x64", "xerces_sax", "data/vod.xml", 13.606);
// xmllite: 8.68 us/byte, 110 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "xmllite", "data/vod.xml", 8.68282);
// xmlparser: 29.33 us/byte, 33 Mb/s, 15709 Kb, 482180 allocations
data("speed", "x64", "xmlparser", "data/vod.xml", 29.3289);
data("memory", "x64", "xmlparser", "data/vod.xml", 15.341);

// data/xmark.xml
// cmarkup: 5.14 us/byte, 185 Mb/s, 29338 Kb, 10 allocations
data("speed", "x64", "cmarkup", "data/xmark.xml", 5.14269);
data("memory", "x64", "cmarkup", "data/xmark.xml", 28.651);
// expat: 5.76 us/byte, 166 Mb/s, 16398 Kb, 118 allocations
data("speed", "x64", "expat", "data/xmark.xml", 5.75856);
// irrxml: 15.43 us/byte, 62 Mb/s, 11604 Kb, 1583123 allocations
data("speed", "x64", "irrxml", "data/xmark.xml", 15.4281);
// ixmlparser_dom: 9.46 us/byte, 101 Mb/s, 29734 Kb, 47 allocations
data("speed", "x64", "ixmlparser_dom", "data/xmark.xml", 9.45723);
data("memory", "x64", "ixmlparser_dom", "data/xmark.xml", 29.0379);
// ixmlparser_pull: 6.41 us/byte, 149 Mb/s, 1024 Kb, 2 allocations
data("speed", "x64", "ixmlparser_pull", "data/xmark.xml", 6.40971);
// libxml_dom: 12.50 us/byte, 76 Mb/s, 109273 Kb, 1109077 allocations
data("speed", "x64", "libxml_dom", "data/xmark.xml", 12.498);
data("memory", "x64", "libxml_dom", "data/xmark.xml", 106.713);
// libxml_sax: 4.04 us/byte, 236 Mb/s, 16401 Kb, 38303 allocations
data("speed", "x64", "libxml_sax", "data/xmark.xml", 4.03715);
// msxml_dom: 25.26 us/byte, 38 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "msxml_dom", "data/xmark.xml", 25.2575);
data("memory", "x64", "msxml_dom", "data/xmark.xml", 37.2109);
// msxml_sax: 10.31 us/byte, 92 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "msxml_sax", "data/xmark.xml", 10.3149);
// pugixml: 1.17 us/byte, 814 Mb/s, 31059 Kb, 607 allocations
data("speed", "x64", "pugixml", "data/xmark.xml", 1.17181);
data("memory", "x64", "pugixml", "data/xmark.xml", 30.3318);
// pugixml_compact: 1.58 us/byte, 604 Mb/s, 15672 Kb, 130 allocations
data("speed", "x64", "pugixml_compact", "data/xmark.xml", 1.5801);
data("memory", "x64", "pugixml_compact", "data/xmark.xml", 15.3055);
// pugxml: 9.46 us/byte, 101 Mb/s, 47414 Kb, 866856 allocations
data("speed", "x64", "pugxml", "data/xmark.xml", 9.46476);
data("memory", "x64", "pugxml", "data/xmark.xml", 46.3031);
// qtxml_dom: 42.44 us/byte, 22 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "qtxml_dom", "data/xmark.xml", 42.4381);
// qtxml_sax: 24.83 us/byte, 38 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "qtxml_sax", "data/xmark.xml", 24.8285);
// rapidxml: 1.39 us/byte, 688 Mb/s, 40477 Kb, 452 allocations
data("speed", "x64", "rapidxml", "data/xmark.xml", 1.38676);
data("memory", "x64", "rapidxml", "data/xmark.xml", 39.5288);
// tinyxml: 40.97 us/byte, 23 Mb/s, 68671 Kb, 1596774 allocations
data("speed", "x64", "tinyxml", "data/xmark.xml", 40.9699);
data("memory", "x64", "tinyxml", "data/xmark.xml", 67.0622);
// tinyxml2: 8.70 us/byte, 110 Mb/s, 42621 Kb, 7827 allocations
data("speed", "x64", "tinyxml2", "data/xmark.xml", 8.70229);
data("memory", "x64", "tinyxml2", "data/xmark.xml", 41.623);
// vtdxml: 4.41 us/byte, 216 Mb/s, 14874 Kb, 63 allocations
data("speed", "x64", "vtdxml", "data/xmark.xml", 4.41199);
data("memory", "x64", "vtdxml", "data/xmark.xml", 14.5261);
// xerces_dom: 11.92 us/byte, 80 Mb/s, 91404 Kb, 18664 allocations
data("speed", "x64", "xerces_dom", "data/xmark.xml", 11.9226);
data("memory", "x64", "xerces_dom", "data/xmark.xml", 89.2626);
// xerces_sax: 7.21 us/byte, 132 Mb/s, 288 Kb, 598 allocations
data("speed", "x64", "xerces_sax", "data/xmark.xml", 7.21222);
// xmllite: 4.38 us/byte, 217 Mb/s, 0 Kb, 0 allocations
data("speed", "x64", "xmllite", "data/xmark.xml", 4.38473);
// xmlparser: 20.21 us/byte, 47 Mb/s, 41385 Kb, 1169012 allocations
data("speed", "x64", "xmlparser", "data/xmark.xml", 20.2079);
data("memory", "x64", "xmlparser", "data/xmark.xml", 40.4151);

} 
