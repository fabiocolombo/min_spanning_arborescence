randomMSA: src/*.cpp include/*.hpp
	$(CXX) src/*.cpp -I include -o randomMSA
clean:
	rm -rf randomMSA
