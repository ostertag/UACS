Here are instructions to convert md file to pdf

Add this to the begin of main.md file:

`---
geometry: a4paper,vmargin=1cm
lang: sk
---`

you can adjust margin and other values if you like

then run with this command:  
make PANDOC_PDF_OPTIONS=--pdf-engine=lualatex

you might need to install soma packages if you working on linux:  
sudo apt-get install texlive-luatex
sudo apt-get install fonts-lmodern
sudo tlmgr install luaotfload
luaotfload-tool --update
