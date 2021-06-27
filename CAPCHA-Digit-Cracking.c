#include <stdio.h>
#include <math.h>
#include "captcha.h"

//functions declared:
double get_vertical_balance(int height, int width, int pixels[height][width]);

int find_holes(int height, int width, int pixels[height][width],
               int begin_pixel[height][width]);

double tallness(int height, int width);

void print_image_numbers(int height, int width, int pixels[height][width]);

double density(int height, int width, int pixels[height][width]);

double widest_row(int height, int width, int pixels[height][width]);


int main(int argc, char *argv[]) {
    
    int height, width, start_row, start_column, box_width, box_height;
    
    int number_of_holes; //retrives how many holes in the image
    
    double balance_topdown; //retrives horizontal balance
    double balance_across; //retrives vertical balance
    
    int hole_box_height;
    int hole_box_width;
    
    int start_row_hole;
    int start_column_hole;
    
    double density_of_image;
    
    double density_of_hole;
    
    double hole_ratio = 0; //counts ratio of black vs white pixels for
                           //numbers 0 and 6

    //for looking at the bottom 1/3 area of image
    int start_bottom_row;
    int start_bottom_column;
    int bottom_box_height;
    int bottom_box_width;
    
    //for looking at the top 1/3 area of image
    int start_top_row;
    int start_top_column;
    int top_box_height;
    int top_box_width;
    
    //for looking at the middle 1/3 area of image
    int start_middle_row;
    int start_middle_column;
    int middle_box_height;
    int middle_box_width;

    if (get_pbm_dimensions(argv[1], &height, &width) != 1) {
        return 1;
    }
        
                
    int pixels[height][width];
    if (read_pbm(argv[1], height, width, pixels)) {
        get_bounding_box(height, width, pixels, &start_row,  
                         &start_column, &box_height, &box_width);
        
        int box_pixels[box_height][box_width];
            copy_pixels(height, width, pixels, start_row, start_column,  
                        box_height, box_width, box_pixels);
                    
            get_bounding_box(box_height/3, box_width, box_pixels,
                             &start_bottom_row, &start_bottom_column,
                             &bottom_box_height, &bottom_box_width);
                    

            //this part finds the bottom 1/3 area of the bounding box image
            int bottom_third_pixels[box_height/3][box_width];
            copy_pixels(height, width, pixels, start_row, start_column,  
                        box_height/3, box_width, bottom_third_pixels);
                    
            get_bounding_box(box_height/3, box_width, bottom_third_pixels,
                             &start_bottom_row, &start_bottom_column,
                             &bottom_box_height, &bottom_box_width);
                    
            //this finds the top 1/3 area of the bounding box image
            int top_third_pixels[box_height/3][box_width];
            copy_pixels(height, width, pixels, start_row + (2*box_height/3),  
                        start_column,box_height/3, box_width, top_third_pixels);
                    
            get_bounding_box(box_height/3, box_width, top_third_pixels,
                             &start_top_row, &start_top_column,
                             &top_box_height, &top_box_width);
                    
            //this finds the middle 1/3 area of the bounding box image
            int middle_pixels[box_height/3][box_width];
            copy_pixels(height, width, pixels, start_row + (box_height/3),
                        start_column, box_height/3, box_width, middle_pixels);
                                
                    
            get_bounding_box(box_height/3, box_width, middle_pixels,
                             &start_middle_row, &start_middle_column,
                             &middle_box_height, &middle_box_width);
                    
            //compares ratio of black pixel vs white pixels
            density_of_image = density(box_height, box_width, box_pixels);
                                               
            //gets horizontal balance
            balance_topdown = get_horizontal_balance(box_height, 
                                                     box_width, 
                                                     box_pixels);
            
            //gets vertical balance
            balance_across = get_vertical_balance(box_height, box_width, 
                                                  box_pixels);
            
            //gets the number of holes
            int holes_pixels[box_height][box_width];    
            number_of_holes = find_holes(box_height, box_width, 
                                         box_pixels, holes_pixels);
            
            //gets the bounding box of the holes in the picture and copies it
            //into new 2d array
            get_bounding_box(box_height, box_width, holes_pixels, 
                             &start_row_hole, &start_column_hole, 
                             &hole_box_height, &hole_box_width);
            

            int bounding_hole_pixels[box_height][box_width];
            
            if(hole_box_height != 0) {
            	copy_pixels(box_height, box_width, holes_pixels, 
                      	  start_row_hole, start_column_hole, 
                      	  hole_box_height, hole_box_width,
                      	  bounding_hole_pixels);
                    
            //get the height and width of the holes
           		density_of_hole = density(hole_box_height, hole_box_width, 
                                    	  bounding_hole_pixels);
            
            }
            
            //calculating digit now:
            if(number_of_holes > 0) {
                if(number_of_holes == 2) {
                    printf("8\n");
                    
                } else {
                    //possible: 4 6 9 0
                    if(start_row_hole > ((double)box_height/4.2)) {
                        //possible: 4 9
                        if(density_of_hole > 0.64) {
                            printf("9\n");
                        } else {
                            printf("4\n");
                        }
                        
                    } else {
                        hole_ratio = (double)hole_box_height/box_height;
                        if(hole_ratio > 0.58) {
                            printf("0\n");
                        } else {
                            printf("6\n");
                        }
    
                    }

                }
            } else {
                //possible: 1 2 3 5 7
                
                //calculates width of each 1/3s of the image
                double bottom_third_width = 0;
                bottom_third_width = (double)box_width/bottom_box_width;

                double top_third_width = 0;
                top_third_width = (double)box_width/top_box_width;

                double middle_third_width = 0;
                middle_third_width = (double)box_width/middle_box_width;
                
                //calculates the ratio of black vs white pixels in centre 1/3
                double density_middle = 0;
                density_middle = density(box_height/3, box_width, 
                                         middle_pixels);
                
                
                if(bottom_third_width >= 1.6) {
                    printf("7\n");
                } else {

                    //turn the left half of the picture black
                    //then count holes
                    for (int y = 0; y < box_height; y++) {
                        for (int x = 0; x < box_width/2; x++) {
                            box_pixels[y][x] = 1;
                        }
                    }
                    int left_holes = find_holes(box_height,
                                                box_width, 
                                                box_pixels,
                                                holes_pixels);

                    double mid_widest_row = widest_row(box_height/3,
                                                       box_width,
                                                       middle_pixels);

                    
                    if(left_holes == 2) {
                        printf("3\n");
                    } else if(left_holes == 0) {
                        printf("1\n");
                    } else {
                        if(mid_widest_row <= 0.6) {
                            printf("2\n");
                        } else {
                            printf("5\n");
                        }
                    }
                }
            }
        }

    return 0;
}


void get_bounding_box(int height, int width, int pixels[height][width],
                      int *start_row, int *start_column, int *box_height, 
                      int *box_width) {

    int x = 0; //width
    int y = height - 1; //height, starting from the top right
    
    //using these starting numbers, because basically any position will be
    //smaller than the starts or larger than the ends
    *start_row = height;
    *start_column = width;
    int end_row = 0;
    int end_column = 0;

    //count from top to bottom, left to right
    while(y >= 0) {
        x = 0;
        while(x < width) {
            if(pixels[y][x] == 1) {
                //if x is bigger than end_row then we update end_row
                if(y > end_row) {
                    end_row = y;
                }
                //likewise for the next three
                if(y < *start_row) {
                    *start_row = y;
                }
                if(x > end_column) {
                    end_column = x;
                }
                if(x < *start_column) {
                    *start_column = x;
                }
            }
            x++;
        }
        y--;
    }
    
    *box_height = end_row - *start_row + 1;
    *box_width = end_column - *start_column + 1;
    
    if(*box_height < 0) {
        *box_height = 0;
    }
    
    if(*box_width < 0) {
        *box_width = 0;
    }
    
}



void copy_pixels(int height, int width, int pixels[height][width],
                 int start_row, int start_column, int copy_height,
                 int copy_width, int copy[copy_height][copy_width]){

    int x = start_column;
    int y = start_row;
    
    int copied_x = 0;
    int copied_y = 0;

    while(copied_y < copy_height){
        while(copied_x < copy_width){
            copy[copied_y][copied_x] = pixels[y][x];
            copied_x++;
            x++;
        }
        copied_y++;
        y++;
        x = start_column;
        copied_x = 0;
    }
}


double get_horizontal_balance(int height, int width, int pixels[height][width]) {
    
    int y = 0; //counts veritcally: | | |
    int x = 0; //counts horizontally: - - -
    
    int column_sum = 0;
    int n_black_pixels = 0;
    double horizontal_balance = 0;
    
    while(x < width) { 
        while(y < height) {
            //direction: down the column and then goes to the 
            //column on the right, repeat
            if(pixels[y][x] == 1) {
                column_sum = column_sum + x;
                n_black_pixels++;
            }
            y++;
        }
        x++;
        y = 0;
    }
    horizontal_balance = (((column_sum/(double)n_black_pixels) + 0.5)/width);
    return horizontal_balance;
}


//gets the vertical balance of the picture, similar to horizontal balance that
//cuts the number in half vertically, this one does so horizontally
double get_vertical_balance(int height, int width, int pixels[height][width]) {
    
    int y = 0; //counts veritcally: | | |
    int x = 0; //counts horizontally: - - -
    
    int row_sum = 0;
    int n_black_pixels = 0;
    double vertical_balance = 0;
    
    while(x < width) { 
        while(y < height) {
            //direction is down and then right
            if(pixels[y][x] == 1) {
                row_sum = row_sum + y;
                n_black_pixels++;
            }
            y++;
        }
        x++;
        y = 0;
    }
    vertical_balance = (((row_sum/(double)n_black_pixels) + 0.5)/height);
    return vertical_balance;
}

//finds the number of holes in a digit with moore neighbouring algorithm
int find_holes(int height, int width, int pixels[height][width],
               int begin_pixel[height][width]) {

    int x = 0; //counts horizontally - - -
    int y = 0; //counts vertically   | | |

    int total_holes = 0;
    
    //stores the position of the first black pixel found for an edge tracing
    int start_x = 0;
    int start_y = 0;
    
    //stores the position of the pixel currently we are circling around
    int current_x = 0;
    int current_y = 0;
    
    //stores the position of the last pixel examined before current one
    int last_x = 0;
    int last_y = 0;
    
    //stores the current boundary pixel
    int boundary_x = 0;
    int boundary_y = 0;
    
    
    //int begin_pixel2[height][width];
    int start_pixel = 0;
    
    int inside_pixel = 8;
    int outside_pixel = 7;
    
    //for bounding box function later
    int start_row; 
    int start_column;
    int box_height; 
    int box_width;
    
    for(y = 0; y < height; y++) {
        for(x = 0; x < width; x++) {
            begin_pixel[y][x] = 0;
        }
    }
    
    x = 0;
    y = 0;
    
    //this loop finds the place to begin
    while(y < height && start_pixel == 0) {
        while(x < width && start_pixel == 0) {
            if(pixels[y][x] == 1) {
                start_x = x;
                start_y = y;
                
                boundary_x = start_x;
                boundary_y = start_y;
                
                begin_pixel[start_y][start_x] = 1;
                start_pixel = 1;
                
            }
            x++;
        }
        x = 0;
        y++;
    }
    
    //writes last position to the one on the left. will check later if it exists
    last_x = start_x - 1;
    last_y = start_y;
    
    //sets current position to the pixel to the one above and one to the left
    current_x = start_x - 1;
    current_y = start_y + 1;
    
   
    while(current_x != start_x || current_y != start_y) {
        //checks for current position to be inside the array dimension and
        //bigger than 0
        //and then checks if is a black pixel
        
        
        if((current_x < width && current_y < height) 
            && (current_x >= 0 && current_y >= 0)
            && (pixels[current_y][current_x]) == 1) {
            
            begin_pixel[current_y][current_x] = 1;

            //now keep track of last black pixel, which is this
            last_x = boundary_x;
            last_y = boundary_y;
            
            boundary_x = current_x;
            boundary_y = current_y;     
        } else {
            last_x = current_x;
            last_y = current_y;
    
        }
            
        //now we set c to the clockwise pixel, with boundary as the centre and
        //last as the previous pixel
        x = last_x - boundary_x;
        y = last_y - boundary_y;
        
        if(x == 0 && y == 1) {
            x = 1;
            y = 1;
            
        } else if(x == 1 && y == 1) {
            x = 1;
            y = 0;  
            
        } else if(x == 1 && y == 0) {
            y = -1;
            x = 1;
            
        } else if(x == 1 && y == -1) {
            y = -1;
            x = 0;
        
        } else if(x == 0 && y == -1) {
            y = -1;
            x = -1;
            
        } else if(x == -1 && y == -1) {
            y = 0;
            x = -1;
            
        } else if(x == -1 && y == 0) {
            y = 1;
            x = -1;
            
        } else if(x == -1 && y == 1) {
            y = 1;
            x = 0;
        }
        
        current_x = boundary_x + x;
        current_y = boundary_y + y;
    }

    //manipulating edge array (begin_pixel)
    //this separates the picture into 3 states: 
    //1. edge pixels - the outline of the number
    //2. inside_pixel - the pixels inside the outline
    //3. outside_pixel - pixels on the outside
    x = 0;
    y = 0;
    
    while(y <height) {
        while(x < width) {
            if(begin_pixel[y][x] != 1) {
                if(x == 0 || begin_pixel[y][x-1] == 1) {
                    if(pixels[y][x] == 1) {
                        begin_pixel[y][x] = inside_pixel;
                    } else {
                        begin_pixel[y][x] = outside_pixel;
                    }
                } else if(begin_pixel[y][x-1] == inside_pixel ||
                          begin_pixel[y][x-1] == outside_pixel) {
                    begin_pixel[y][x] = begin_pixel[y][x-1];
                }
                
            }
            x++;
        }
        x = 0;
        y++;
    }
    
    //print_image_numbers(height, width, begin_pixel);
    
    y = 0;
    x = 0;
    //inverts the inside pixels and edge pixels
    while(y < height) {
        while(x < width) {
            if(begin_pixel[y][x] == outside_pixel) {
                begin_pixel[y][x] = 0;
            } else {
                begin_pixel[y][x] = 1 - pixels[y][x];
            }
            x++;
        }
        y++;
        x = 0;
    }

    //this is the bounding box of the hole(s)
    get_bounding_box(height, width, begin_pixel, &start_row, &start_column, 
                         &box_height, &box_width);
    
    
    
    //printf("box_height: %d\n box_width: %d\n", box_height, box_width);
    if(box_height != 0) {
    	int box_pixels[box_height][box_width];
    	copy_pixels(height, width, begin_pixel, start_row, start_column, 
               		box_height, box_width, box_pixels);
    
		//print_image(box_height, box_width, box_pixels);
		
		//checking if one hole or two hole by seeing if there is a white row
		x = 0;
		y = 0;
		
		while(y < box_height) {
			while (x < box_width) {
				if(box_pixels[y][x] == 1) {
					break;
				} else {
					//if a whole row of it is white:
					if(x == box_width - 1) {
						total_holes = 2;
					}
				}
				x++;
			}
			y++;
			x = 0;
		}
		
		//checking if total_holes, when not 2, whether should be to 1 or 0
	   
		//<3 to eliminate cases where just a single box or two comes up as a '
		//hole'
		if(box_height < 3 || box_width < 3) {
			total_holes = 0;
		} else if(total_holes == 2) {
			total_holes = 2;
		} else {
			total_holes = 1;
		}
	} else {
		total_holes = 0;
	}
    
    return total_holes;
}

//prints an image in *'s and .'s
void print_image(int height, int width, int pixels[height][width]) {
    
    int countHeight = height - 1;
    int countWidth = 0;

    
    while(countHeight >= 0) {
        while(countWidth < width) {
            if (pixels[countHeight][countWidth] == 0) {
                printf(".");
                countWidth++;
            } else if (pixels[countHeight][countWidth] == 1) {
                printf("*");
                countWidth++;   
            }
        }
        printf("\n");
        countWidth = 0;
        countHeight--;
    }   
}

//prints an image in numbers instead of *'s and .'s 
//This was for the holes numbers 4 6 9 0
void print_image_numbers(int height, int width, int pixels[height][width]) {

    int countHeight = height - 1;
    int countWidth = 0;

    
    while(countHeight >= 0) {
        while(countWidth < width) {
            printf("%d", pixels[countHeight][countWidth]);
            countWidth++;
        }
        printf("\n");
        countWidth = 0;
        countHeight--;
    }   
}

//measures the height divided by width of a number
double tallness(int height, int width) {
    double x = 0;
    x = (double)height/width;
    
    return x;
}

//black vs white pixel ratio given for a hole
//used for numbers 4 6 9 0 (the ones with holes)
double density(int height, int width, int pixels[height][width]) {
    int x = 0;
    int y = 0;
    
    int hole_size = 0;
    
    double density = 0;
    
    while(y < height) {
        while(x < width) {
            if(pixels[y][x] == 1) {
                hole_size++;
            }
            x++;
        }
        x = 0;
        y++;
    }
    
    density = ((double)hole_size)/(height*width);
    return density;
}

//searches for the widest row in a given area. Used this for differentiating
//5 and 2
double widest_row(int height, int width, int pixels[height][width]) {
    int x;
    int y;
    
    int farthest_left = 0;
    int farthest_right = 0;
    
    int row_width = 0;
    
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            if (pixels[y][x] == 1) {
                farthest_left = x;
                break;
            }
        }
        for (x = width-1; x >= 0; x--) {
            if (pixels[y][x] == 1) {
                farthest_right = x;
                break;
            }
        }
        row_width = fmax(farthest_right - farthest_left, row_width);
    }
    
    return (double)row_width/width;
}
