/** Image Alignment **/
program: /src/align.cpp
environmet: VS2015, c++, OpenCV 3.1.0
how to program:
1. Prepare a list.txt file whose first line is the directory containing input images, and the second line is the directory where the aligned images will be saved. The other lines are the image names in the order of descending exposure times.
(For example:  /input_image/imageSet1/list.txt )
2. Prepare a time.txt listing the exposure times in a descending order.
 (For example:  /input_image/imageSet1/time.txt )
3. Execute the align.cpp, and then it would ask you to input the file (list.txt) you want the program to read.
4. After the program ends, you can find aligned images saved in the directory you have pointed at in list.txt.

/** HDR assembling **/
program: /src/hdr.m /src/gsolve.m
environmet: MATLAB_2013a
how to program:
1. Prepare a list.txt file whose first line is the directory containing input images, and the other lines are the aligned image names in the order of descending exposure times.
2. Prepare a time.txt listing the exposure times in a descending order.
 (For example:  /input_image/imageSet1/time.txt )
3. Call the function hdr in MATLAB, the input parameters are the list.txt and time.txt file, and the output will return hdr image.

/** tone mapping by reinhard **/
program: /src/reinhard.cpp
environmet: VS2015, c++, OpenCV 3.1.0
how to program:
1.Execute the program and input the hdr image file(.jpg) you want to apply tone mapping to and enter the final image file.
2. After the program ends, you will find the output image file.