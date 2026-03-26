To convert .md file to .pdf file start by adding:

--- geometry: a4paper,vmargin=1cm lang: sk ---

to the beginning of the .md file

then run:
make filename.pdf

you might need to install your wanted language using: sudo apt-get install texlive-lang-your_language or some additional packages, for example: sudo apt-get install texlive-luatex
