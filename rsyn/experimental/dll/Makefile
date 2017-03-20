prog: main.cpp lib.so
	$(CXX) $(CXXFLAGS) -std=c++11 -o prog main.cpp -ldl -pthread

lib.so: lib.cpp
	$(CXX) $(CXXFLAGS) -std=c++11 `wx-config --debug=no --cxxflags --libs` -fPIC -shared -o lib.so lib.cpp

clean:
	rm -f prog lib.so

.PHONY: clean
