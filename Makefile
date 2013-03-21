soi:
	node-gyp --python python2 configure build
	mv build/*/soi.node .

clean:
	rm -f *.o soi.node
	rm -Rf build