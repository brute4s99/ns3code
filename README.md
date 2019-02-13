## Use a system with Ubuntu 16.04 (WSL works too)
1. Get [ns3.24.1](https://nsnam.org) from [here](https://www.nsnam.org/releases/older/).
2. extract the folder and run `./build.py` after installing all deps (~10 minutes on i5-6200U with 8GB RAM)
> sudo apt-get install gcc g++ python python-dev mercurial bzr gdb valgrind gsl-bin libgsl0-dev libgsl2 flex bison tcpdump sqlite sqlite3 libsqlite3-dev libxml2 libxml2-dev libgtk2.0-0 libgtk2.0-dev uncrustify doxygen graphviz imagemagick texlive texlive-latex-extra texlive-generic-extra texlive-generic-recommended texinfo dia texlive texlive-latex-extra texlive-extra-utils texlive-generic-recommended texi2html python-pygraphviz python-kiwi python-pygoocanvas libgoocanvas-dev python-pygccxml

3. run `./build.py --enable-examples --enable-tests`
4. That's it! You can load the codes of this repo in the `./ns-3.24.1/scratch` folder now!

>DEV tip : merge the scratch folder with a clone of this repo

5. To run a code, `./waf --run <file_name_without_extension>`