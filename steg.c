#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <stdbool.h>

#define BUFFER 10000 //Defines a buffer for any char *array allocations

/*---------------------------------------------------------------------------- Pixel struct ----------------------------------------------------------------------------*/
/* The RGB values of a pixel. */
struct Pixel {
    int red;
    int green;
    int blue;
    //bool isVisited;
};
typedef struct Pixel PIXEL;



/*---------------------------------------------------------------------------- LinkedList Node struct ----------------------------------------------------------------------------*/
struct Node {
	char *value;
	struct Node *next;
};
typedef struct Node NODE;



/*---------------------------------------------------------------------------- PPM struct ----------------------------------------------------------------------------*/
/* An image loaded from a PPM file. */
struct PPM {
    char *Format;
    NODE *comments;
    int commentCounter;
    int width, height;
    int max;
    PIXEL **pixelArray;
};
typedef struct PPM PPM;



/*---------------------------------------------------------------------------- ALL Function Prototypes ----------------------------------------------------------------------------*/
PPM* getPPM(FILE * f);
void showPPM(const PPM *img);
PPM* readPPM(const char *filename);
void getComments(FILE *f, PPM *img);
char* readLine(FILE *fin);
int* toAscii(const char *text);
PPM* encode(const char *text, const PPM *img);
char* decode(const PPM *oldimg, const PPM *newimg);


   	
/*---------------------------------------------------------------------------- getPPM Function ----------------------------------------------------------------------------*/
/* Reads an image from an open PPM file.
 * Returns a new struct PPM, or NULL if the image cannot be read. */
PPM* getPPM(FILE * f)
{
    if (f == NULL) { fprintf(stderr,"ERROR! Could not read file"); return NULL; } //Checks if image was able to be read
    
    PPM *allocPPM = malloc(sizeof(PPM)); //Allocate memory for PPM file being read into the PPM struct
    
    //FORMAT
    allocPPM->Format = malloc(3 * sizeof(char));
    allocPPM->Format = readLine(f);
    
    //COMMENTS
    allocPPM->commentCounter = 0;
    getComments(f, allocPPM);
    
    //SIZES
    fscanf(f, "%d", &allocPPM->width);
    fscanf(f, "%d", &allocPPM->height);
    fscanf(f, "%d", &allocPPM->max);
    
    //PIXELS
    allocPPM->pixelArray = calloc(allocPPM->height, sizeof(PIXEL)); //Allocates memory for the "rows" of 2D Array 'pixelArray'
    for(int i=0 ; i<allocPPM->height ; i++)
    {
    	allocPPM->pixelArray[i] = calloc(allocPPM->width, sizeof(PIXEL)); //Allocates memory for the "columns" of 2D Array 'pixelArray'
    	for(int j=0 ; j<allocPPM->width ; j++)
    	{
    	    fscanf(f, "%d", &(allocPPM->pixelArray[i][j].red));
    	    fscanf(f, "%d", &(allocPPM->pixelArray[i][j].green));
    	    fscanf(f, "%d", &(allocPPM->pixelArray[i][j].blue));
    	}
    }
    
    return allocPPM;	
}



/*---------------------------------------------------------------------------- showPPM Function ----------------------------------------------------------------------------*/
/* Write img to stdout in PPM format. */
void showPPM(const PPM *img)
{
    //FORMAT
    fprintf(stdout, "%s\n", img->Format);
    
    //COMMENTS
    NODE *currentNode = img->comments;
    if(currentNode != NULL) {
    	int counter = img->commentCounter;
    	fprintf(stdout, "%s\n", currentNode->value);
    	for(int nodeNum=1 ; nodeNum<counter ; nodeNum++)
    	{
    	    currentNode = currentNode->next;
    	    fprintf(stdout, "%s\n", currentNode->value);
    	}
    }
    
    //SIZES
    fprintf(stdout, "%d %d\n", img->width, img->height);
    fprintf(stdout, "%d\n", img->max);
    
    //PIXELS
    for(int i=0 ; i<img->height ; i++)
    {
	for(int j=0 ; j<img->width ; j++)
	{
            PIXEL pixel = img->pixelArray[i][j];
            fprintf(stdout, "%d %d %d\n", pixel.red, pixel.green, pixel.blue);
        }
    }
}



/*---------------------------------------------------------------------------- readPPM Function ----------------------------------------------------------------------------*/
/* Opens and reads a PPM file, returning a pointer to a new struct PPM.
 * On error, prints an error message and returns NULL. */
PPM* readPPM(const char *filename)
{
    /* Open the file for reading */
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        fprintf(stderr, "File %s could not be opened.\n", filename);
        return NULL;
    }

    /* Load the image using getPPM */
    PPM *img = getPPM(f);

    /* Close the file */
    fclose(f);

    if (img == NULL) {
        fprintf(stderr, "File %s could not be read.\n", filename);
        return NULL;
    }

    return img;
}



/*---------------------------------------------------------------------------- helper Functions ----------------------------------------------------------------------------*/

/*-------------------- getComments Function --------------------*/
//Gets comments from file f and inputs them into PPM struct using LinkedLists
void getComments(FILE *f, PPM *img)
{
    char chr = getc(f);
    while(chr == '#')
    {
    	//Gets comment with #
    	ungetc(chr, f);
    	char *line = readLine(f);
    	
    	//ADD COMMENT TO LINKED LIST
    	if(img->commentCounter == 0) { //Head Node
    	    img->comments = malloc(sizeof(NODE));
    	    img->comments->value = line;
    	    img->commentCounter = 1;
    	} else { //Next Node
    	    NODE *currentNode = img->comments;
    	    for(int count=1 ; count<img->commentCounter ; count++)
    	    {
    	    	currentNode = currentNode->next;
    	    }
    	    currentNode->next = malloc(sizeof(NODE));
    	    currentNode->next->value = line;
    	    img->commentCounter = img->commentCounter + 1;
    	}
    	//Move to next line
    	chr = getc(f);
    }
    //Return to starting position
    ungetc(chr, f);
}


/*-------------------- readLine Function --------------------*/
//Reads line from file with arbitrary number of characters
char* readLine(FILE *fin)
{
    char dummy[BUFFER]; //Dummy string used so line is not over-allocated
    char *line;
    char chr;
    int count = 0;
    
    chr = getc(fin);
    
    if (chr == EOF) { return NULL; } //Empty file checker

    //Populate dummy char by char
    while (chr!='\n' && count < BUFFER-1)
    {
	dummy[count] = chr;
	count++;
	chr = getc(fin);
    }
    dummy[count] = '\0';
    
    //Populating line from dummy
    line = calloc((count+1), sizeof(char));
    for(int i=0 ; i<count ; i++)
    {
	line[i] = dummy[i];
    }
    line[count] = '\0';
    return line;
    
    free(dummy); //Free dummy to get rid of overly-allocated string
}


/*-------------------- toAscii Function --------------------*/
//Converts a string to an integer array of the ASCII values of each character in the string
int* toAscii(const char *text)
{
    int len = strlen(text);
    int *ascii;
    ascii = malloc(len * sizeof(int));
    for(int i=0 ; i<len ; i++)
    {
    	ascii[i] = (int) text[i];
    }
    return ascii;
}



/*---------------------------------------------------------------------------- encode Function ----------------------------------------------------------------------------*/
/* Encode the string text into the red channel of image img.
 * Returns a new struct PPM, or NULL on error. */
PPM* encode(const char *text, const PPM *img)
{
    //ERROR CHECK
    if(img == NULL){
    	fprintf(stderr, "PPM is empty\n");
    	return NULL;
    }
    
    //DECLARATIONS
    int random;
    int width = img->width;
    int height = img->height;
    int size = width * height;
    int arrLen = strlen(text);
    int *asciiArray = toAscii(text);
    
    //COMPATABILITY CHECK
    if( (arrLen * 50) < (width * height) ) {
    	fprintf(stderr, "> File size OK...\n");
    } else {
    	fprintf(stderr, "ERROR! File size not acceptable for encoding\n");
    	return NULL;
    }
    
    //RANDOM START POINT FOR ENCODING
    random = (rand() % width); //rand() is modded by width so that the value generated is always less than the width
    int row, column;
    row = 0;
    column = random;
    
    //ENCODING
    //REPLACEMENT IMPLEMENTATION
    for(int i=0 ; i<arrLen ; ) //Loop for each character in text to be encoded into PPM file. 'i' is only incremented when character is successfully encoded.
    {
    	if(row >= height && column >= width){
    	    row = 0; 
    	    column = 0;
    	}
    	else if(column >= width){
    	    row++;
    	    column = 0;
    	} else {
    	    PIXEL *pixel = &(img->pixelArray[row][column]);
    	    if(pixel->red != asciiArray[i]) {
    	    	pixel->red = asciiArray[i++];
    	    	column += random;
    	    } else { //Try the next pixel, if the char ASCII is == to the red value.
    	    	column++;
    	    }
    	}
    }
    
    return img;    	
}


/*---------------------------------------------------------------------------- decode Function ----------------------------------------------------------------------------*/
/* Extract the string encoded in the red channel of newimg, by comparing it
 * with oldimg. The two images must have the same size.
 * Returns a new C string, or NULL on error. */
char* decode(const PPM *oldimg, const PPM *newimg)
{
    //DECLARATIONS
    //oldimg DECLARATIONS
    int oldWidth = oldimg->width;
    int oldHeight = oldimg->height;
    int oldMax = oldimg->max;
    //newimg DECLARATIONS
    int newWidth = newimg->width;
    int newHeight = newimg->height;
    int newMax = newimg->max;
    //message DECLARATION
    char *decodedText = calloc(BUFFER, sizeof(char));
    int count = 0;
    
    //DECODING
    if( !(oldWidth == newWidth && oldHeight == newHeight && oldMax == newMax) ) { //Check if the the files are compatible for decoding.
    	fprintf(stderr, "ERROR! PPM files do not match\n");
    	return NULL;
    }
    //COMPARISON IMPLEMENTATION - Problem with the order of the message (encoded message: Hamza ||||| decoded message: azamH)
    for(int r=0 ; r<newHeight ; r++) //Each ROW
    {
    	for(int c=0 ; c<newWidth ; c++) //Each COLUMN
    	{
    	    PIXEL *oldPixel = &(oldimg->pixelArray[r][c]);
    	    PIXEL *newPixel = &(newimg->pixelArray[r][c]);
    	    if(oldPixel->red != newPixel->red) {
    	    	decodedText[count] = (char) newPixel->red;
    	    	count++;
    	    }
    	}
    	if(decodedText[count] == '\n') {
    	    decodedText[count] == '\0';
    	    break;
    	}
    }
    	    
    return decodedText;
}





/*---------------------------------------------------------------------------- main Function ----------------------------------------------------------------------------*/
/* TODO: Question 3 */
int main(int argc, char *argv[])
{
    /* Initialise the random number generator, using the time as the seed */
    srand(time(NULL));

    /* Parse command-line arguments */
    
    //TEST - Test read and write of PPM into struct
    if (argc == 3 && strcmp(argv[1], "t") == 0) {
        PPM *img = readPPM(argv[2]);
        showPPM(img);
    } 
    
    //ENCODING - Encode message into PPM file
    else if (argc == 3 && strcmp(argv[1], "e") == 0) {

        PPM *oldimg = readPPM(argv[2]);
        
        if(oldimg == NULL) {
            return 1;
        }
	
	char text[BUFFER];
        fprintf(stderr, "Message to Encode-> ");
	fgets(text, BUFFER, stdin);

        PPM *newimg;
        newimg = encode(text, oldimg);
        
        if(newimg == NULL) {
            fprintf(stderr, "ERROR during Encoding\n");
            return 1;
        } else {
            fprintf(stderr, "Encoding in progress...\n");
        }
        
        showPPM(newimg);
        fprintf(stderr, "Encoding successful\n");
    } 
    
    //DECODING - Decode message from file created in encoding
    else if (argc == 4 && strcmp(argv[1], "d") == 0) {
        PPM *oldimg;
        oldimg = readPPM(argv[2]);

        PPM *newimg;
       	newimg = readPPM(argv[3]);

        char *message;
        message = decode(oldimg, newimg);

        fprintf(stderr, "The message encoded is:\n%s\n", message);

    } 
    
    //INCORRECT ARGUMENTS
    else {
        fprintf(stderr, "Unrecognised or incomplete command line.\n");
        return 1;
    }

    return 0;
}

