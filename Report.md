# F28HS - Hardware-Software Interface Coursework 1 

### *By: Hamza Hassan Mooraj (H00390746)*

## How to Use

To encode a message into a PPM image, the following command is used:

    ./steg e input.ppm > out.ppm
After executing the command, you will be prompted to enter a message which is then encoded. The encoded PPM is printed to stdout; it is redirected to a file out.ppm.

To decode, you enter the command:

    ./steg d input.ppm out.ppm
The command is followed by the message being printed to the terminal.

steg.c is only meant to be used with plain PPM images. They have the following format:
> P3
> #comment 1
> ...
> #comment n
> width height
> max
> r1 g1 b1
> r2 g2 b2
> r3 g3 b3
> ...

The pixels are represented as decimal values from 0 to max, to represent the RGB value for each colour in a pixel.

## Program Design

steg.c is a program that encodes a message into a PPM file by using the ASCII values of the characters in the text and the red value of the pixels in the PPM file.

The program contains functions and algorithms to encode text *to* the PPM, decode text *from* a PPM, and print a PPM to the terminal. It also contains functions that complete the program's 'circuit' under the hood.

Various design choices had to be made to make the program more usable, and better functioning. These choices are specified after the explanation of algorithms used.

## Choice of Data Structures

A struct is used to represent the PPM file information and the pixels in the PPM file. A struct is also used to create a Linked List for any comments written in the PPM file. The implementation of all three structs are as follows:

``struct Pixel`` consists of:
 1. **Integer red**: An integer value holding the red RGB value of the pixel.
 2. **Integer green**: An integer value holding the green RGB value of the pixel.
 3. **Integer blue**: An integer value holding the blue RGB value of the pixel.
***(struct Pixel is written as PIXEL in the rest of this report)***

``struct Node`` consists of:
 1. **String value**: The value of the node in the linked list is represented as a string.
 2. **Node next**: The pointer of the node that points at the next node in the linked list.
***(struct Node is written as NODE in the rest of this report)***

``struct PPM`` consists of:
 1. **String Format**: A string that holds the format of the PPM File (i.e. P3)
 2. **NODE comments**: A linked list containing the comments written in the PPM file.
 3. **Integer commentCounter**: An integer value that holds the number of comments stored in the linked list.
 4. **Integer width**: An integer value of the number of pixels width-wise of the PPM file.
 5. **Integer height**: An integer value of the number of pixels height-wise of the PPM file.
 6. **Integer max**: An integer value of the maximum value of the decimal values of all three RGB values of each pixel.
 7. **PIXEL pixelArray**: This is a 2D array that stores the RGB values in each pixel of the PPM file. It is stored such that there is an array of the three RGB values within each element of the array of pixels.
***(For the rest of the report, I will refer to struct PPM as 'PPM', and a PPM file as 'PPM file')***

## Choice of Algorithms

steg.c uses defined functions that are used in unison to create the functionality of the program. There are also some design choices made in steg.c that will be justified below.

While there are many functions used in steg.c, the functionality and use of some are not as obvious as others.
Some of the obvious functions are:
 - ``getPPM`` - Reads the values from an open PPM file and stores them in the according fields of PPM. It does use some algorithms which are explained in detail further on in this report.
 - ``showPPM`` - Writes the PPM to stdout.
 - ``readPPM`` - Opens and reads a PPM file, and stores it into the PPM using getPPM

The other functions are more complicated in terms of how and why they are used, and what is going on under the hood.
These functions are as follows:

---
    char* readLine(FILE *f)
The readLine function is used to read a single line of characters from an open file and store it into a string. 

A dummy array is used to store the line character by character. This array has a size of 1000 characters. The array is populated until a 'next line' character ``\n`` is reached. Each iteration of the above is counted using an integer variable counter.

Then ``char *line`` is defined and is allocated count + 1 space. It is then populated from the dummy array character by character. 

``char *line`` is returned, and the space holding the dummy array is freed.

---
    void getComments(FILE *f, PPM *img)
This function is used to populate the linked list defined in PPM with comments written in the PPM file. 

A loop checks if the character retrieved, using ``getc()``, is a '#'. If it is, then it is a comment. 

Within the loop:
- The line with the comment is retrieved using ``readLine()``, and is stored in ``char *line``. 
- The next step is to check if commentCounter (from PPM) is 0.
	- If it is 0, we allocate memory for the headnode of the linked list.
	- The comment stored in ``char *line`` is added to headnode.
	- commentCounter is then set to 1 for the next iteration.
- In the next iteration, commentCounter is 1.
	- A for loop is set up to traverse the linked list to the last node that is not null.
	- The next node has memory allocated to it, and has the comment stored in ``char *line`` added to its value.

---
    int* toAscii(const char *text)
A simple function that takes a string, converts each character in the string into its ASCII counterpart, and then stores the ASCII into an integer array.

It uses a for loop to iterate through the string character by character using a built-in function ``strlen()`` to set the limit of the for loop.

---
	PPM* encode(const char *text, const PPM *img)
This is one of the more significant functions in this program. The function takes a string and a PPM as arguments and returns a PPM. The plan to encode the string into the PPM is to replace the red value of the pixel with the ASCII value of the character. This is done for every character, going from left to right on the PPM.

 - The first step is the use of the ``toAscii()`` function to convert the string into an integer array of the character's ASCII values.
 - The first pixel to be encoded is on the 0th row and a random column *(row and column are integer definitions)*.
 - A for loop is run for the length of the integer array. 
	 - Within the loop, there are checks to make sure that the row and column selected is within the PPM.
	 - The red value is then replaced with the ASCII value. If the red value of the pixel is equal to the ASCII value, then the adjacent pixel will be selected.
	 - The column is incremented by a random amount before the next iteration.
- The new PPM with the string encoded in it is then returned.

---
	char* decode(const PPM *oldimg, const PPM *newimg)
This function takes the original PPM (oldimg) and the PPM with the string encoded in it (newimg) as arguments and returns a string (the encoded message). There is an integer count that keeps track of the index of the string to be returned. The decoding starts from the 0th row and the 0th column.

- The traversal of the pixels in the PPM is a nested for loop, which is limited by the height and width of the PPM.
- The red value of the selected pixel of oldimg *(at position T)* is compared with the red value of the selected pixel of newimg *(also at position T)*. 
	- If they are different, then the red value of newimg is casted as a character and added to the index count of the string to be returned. count is then incremented.
	- If they are equal, then that pixel has not been encoded, so the program continues to the next iteration.
- Finally, after every pixel is checked, the string with the encoded message is returned.

## Design Choices

Some of the algorithms above have specific design choices that aid in the programs overall functionality and complexities.
- In ``readLine()``, the use of a dummy array allows for an undefined number of characters to be read from the file, without using too much extra space. The dummy array having a size of BUFFER (which is defined as 10000) has too much allocated space for a string of 10 characters. Therefore, allocating 10 chars for the actual array and freeing up the space used by the dummy array improves the space complexity of the program. This function was made rather than using ``getLine()``, as there are an arbitrary number of characters in each line, and so the length of the array necessary can be decided at runtime rather than compile-time.
- The ``getComments`` function is used to reduce clutter in the ``getPPM()`` function.
- The encoding starts in the 0th row, so that there is plenty of space to encode a long message into the PPM. The compatibility check in the ``encode()`` function is to ensure that the message is small enough to be encoded and that the encoding is not too obvious. The ``rand()`` is modded by the width to keep the value less than the width at all times.
- The decoding requires a nested for loop since the pixels in the PPM are a 2D array of rows and columns. The use of count allows the program to insert the character in the correct position of the string. The count value is then used to insert the string ending identifier, ``\0`` in place of ``\n``.

There are other design choices that are not nested in the algorithms used:
- The use of ``stderr`` for printing any text to the terminal, as ``stdout`` would be directed to the out.ppm file.
- The implementation of prototypes at the beginning of the program source code allows for a more readable source code, and allows use of functions before implementation.