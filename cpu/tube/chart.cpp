// cpu/tube/chart.cpp -*-C++-*-
// ----------------------------------------------------------------------------
//  Copyright (C) 2014 Dietmar Kuehl http://www.dietmar-kuehl.de         
//                                                                       
//  Permission is hereby granted, free of charge, to any person          
//  obtaining a copy of this software and associated documentation       
//  files (the "Software"), to deal in the Software without restriction, 
//  including without limitation the rights to use, copy, modify,        
//  merge, publish, distribute, sublicense, and/or sell copies of        
//  the Software, and to permit persons to whom the Software is          
//  furnished to do so, subject to the following conditions:             
//                                                                       
//  The above copyright notice and this permission notice shall be       
//  included in all copies or substantial portions of the Software.      
//                                                                       
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,      
//  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES      
//  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND             
//  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT          
//  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,         
//  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING         
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR        
//  OTHER DEALINGS IN THE SOFTWARE. 
// ----------------------------------------------------------------------------

#include <iostream>
#include <limits>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <stdexcept>

// ----------------------------------------------------------------------------

namespace
{
    void erase(std::string& value, std::string const& remove)
    {
        std::string::size_type pos(value.find(remove));
        if (pos != value.npos) {
            value.erase(pos, remove.size());
        }
    }
}

// ----------------------------------------------------------------------------

int main(int ac, char* av[])
{
    try
    {
        std::streamsize const max(std::numeric_limits<std::streamsize>::max());
        std::vector<std::string>                        order;
        std::vector<std::string>                        system;
        std::map<std::string, std::vector<std::string> > values;

        if (ac < 2) {
            throw std::runtime_error("usage: " + std::string(av[0]) + " <name> [<results>*]");
        }
        std::string name(av[1]);

        for (int i(2); i != ac; ++i) {
            std::ifstream in(av[i]);
            if (!in) {
                throw std::runtime_error("failed to open file '" + std::string(av[i]) + "' for reading");
            }
            std::string tmp;
            if (std::getline(in, tmp)) {
                erase(tmp, "arch=");
                erase(tmp, "processor=");
                erase(tmp, "compiler=");
                erase(tmp, "flags=");
                system.push_back(tmp);
            }
            for (std::string test, result;
                 std::getline(std::getline(in, test, '|'), result, ',').ignore(max, '\n'); ) {
                std::map<std::string, std::vector<std::string> >::iterator it(values.find(test));
                if (it == values.end()) {
                    order.push_back(test);
                    it = values.insert(std::make_pair(test, std::vector<std::string>())).first;
                }
                it->second.resize(system.size() - 1u);
                it->second.push_back(result);
            }
        }

        std::ofstream out(("charts/" + name + ".html").c_str());

        out << "<html>\n";
        out << "  <head>\n";
        out << "    <meta charset=\"UTF-8\">\n";
        out << "    <script type=\"text/javascript\" src=\"https://www.google.com/jsapi\"></script>\n";
        out << "    <script type=\"text/javascript\">\n";
        out << "      google.load(\"visualization\", \"1\", {packages:[\"corechart\"]});\n";
        out << "      google.setOnLoadCallback(drawChart);\n";
        out << "      function drawChart() {\n";
        out << "        var data = google.visualization.arrayToDataTable([\n";

        out << "[ 'Test'";
        for (std::vector<std::string>::const_iterator it(system.begin()), end(system.end());
             it != end; ++it) {
            out << ", '" << *it << "'";
        }
        out << "],\n";
        for (std::vector<std::string>::const_iterator it(order.begin()), end(order.end());
             it != end; ++it) {
            out << "['" << *it << "'";
            std::vector<std::string>& results(values[*it]);
            results.resize(system.size());
            for (std::vector<std::string>::const_iterator rit(results.begin()), rend(results.end());
                 rit != rend; ++rit) {
                 out << ", " << *rit;
            }
            out << "]" << (&*it == &order.back()? "": ",") << '\n';
        }
        out << "]);\n";

        out << "        var options = {\n";
        out << "          chartArea: { width: \"60%\", height: \"95%\" },\n";
        out << "          title: '" << name << "',\n";
        out << "          hAxis: {title: 'time'}\n";
        out << "        };\n";
        out << "\n";
        out << "        var chart = new google.visualization.BarChart(document.getElementById('chart_div'));\n";
        out << "\n";
        out << "        chart.draw(data, options);\n";
        out << "      }\n";
        out << "    </script>\n";
        out << "  </head>\n";
        out << "  <body>\n";
        out << std::ifstream(("cpu/test/" + name + ".html").c_str()).rdbuf();
        out.clear();
        out << "    <div id=\"chart_div\" style=\"width: 1400px; height: "
            << (12 * system.size() * order.size()) << "px;\"></div>\n";
        out << "  </body>\n";
        out << "</html>\n";
    }
    catch (std::exception const& ex)
    {
        std::cout << "ERROR: " << ex.what() << '\n';
    }
}
