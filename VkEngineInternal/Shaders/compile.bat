%~dp0/glslc.exe shader.vert -o vert.spv
%~dp0/glslc.exe shader.frag -o frag.spv

%~dp0/glslc.exe shader-tile.vert -o vert-tile.spv
%~dp0/glslc.exe shader-tile.frag -o frag-tile.spv

%~dp0/glslc.exe shader-ui.vert -o vert-ui.spv
%~dp0/glslc.exe shader-ui.frag -o frag-ui.spv

pause