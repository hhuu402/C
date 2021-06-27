#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "trader_bot.h"

//allocating some space for array
#define SOME_SPACE 50
//for moving 0 spaces
#define NOTHING 0

struct information {
    //using an array to record the prices at every petrol station
    int station_price[SOME_SPACE];
    
};

//all function purposes are at the bottom (sitting on where the functions are)

int total_turns(struct bot *b);

int count_bot(struct bot *b);

int how_much_buy(struct bot *b, int subtraction);

int determin_world_size(struct bot *b);

int check_distance(struct bot *b, int type, char *commodity, int distance);

int fuel_price(struct information *use, struct bot *b);

int max_buy(struct bot *b);

//name of the robot: KHAJIIT HAS WARES
char *get_bot_name(void) {
    return "--KHAJIIT HAS WARES--";
}

void get_action(struct bot *b, int *action, int *n) {
    *action = ACTION_MOVE;
    *n = 1;

    struct information *use = malloc(sizeof(struct information));

    //decided to use another name for b->location
    struct location *where_bot = b->location;

    //these are the scouts for looking around where my bot is
    struct location *going_front;
    struct location *going_back;
    struct location *am_here;
    //scout here is a counter so I know how many spaces to move
    int scout = 0;
    int scout2 = 0;

    //get the highest price of the fuel
    int highest_price = fuel_price(use, b);

    //carries the world's size that is calcuated from functions
    //count_bot, determin_world_size
    int world_size;

    //if location is at start, dump, or other
    if(where_bot->type == LOCATION_START || where_bot->type == LOCATION_DUMP
        || where_bot->type == LOCATION_OTHER) {

        //setting my scouts' locations
    	going_front = b->location;
    	going_back = b->location;

        //is there at least half a tank of fuel left?
    	if(b->fuel <= scout+(2*b->maximum_move)) {
        	//no. go to petrol station!! Start by searching:
    		scout = check_distance(b, LOCATION_PETROL_STATION, "NULL", b->fuel);

            //if the petrol station is out of reach
            //from how much fuel I have left...
    		if(scout >= b->fuel) {
    			//E M E R G E N C Y   P R O C E E D U R E
    			//A C T I V I A T E  :  S E L L
                //E V E R Y T H I N G
    			
                if(b->cargo != NULL && b->cargo->quantity > 0) {
    				scout = check_distance(b, LOCATION_BUYER, 
                            b->cargo->commodity->name, b->fuel);
                    
                    *action = ACTION_MOVE;
                    *n = scout;
                    return;
    			} else {
                //cargo is empty, going to buy some items with the last
                //of this bot's breath and sell it :(
                    if(b->turns_left >= total_turns(b)) {
                        scout = check_distance(b, LOCATION_SELLER, "NULL", b->fuel);
                        *action = ACTION_MOVE;
                        *n = scout;
                        return;
                    } else {
                        *action = ACTION_MOVE;
                        *n = NOTHING;
                    }

                }
   
            } else {
                //it needs the petrol station
                *action = ACTION_MOVE;
                *n = scout;
                return;
                
   			}

		} else {
            //fuel is good, now see if anything need to be sold,
            //check if cargo is used and filled with similar items
            if(b->cargo != NULL && b->cargo->quantity > 0) {
    			//yes it is
                scout = check_distance(b,LOCATION_BUYER, 
                        b->cargo->commodity->name,b->fuel);

                *action = ACTION_MOVE;
                *n = scout;
                return;

    		} else {
                //no it isn't
                scout = check_distance(b, LOCATION_SELLER, "NULL", b->fuel);
                //either it finds a seller, or it doesn't.
                //it doesn't matter because either I move to the seller or
                //maximum move which will take me elsewhere from this depleted
                //land
                *action = ACTION_MOVE;
                *n = scout;
                return;
    		}
   
    	}

    //current location is a seller
    } else if(where_bot->type == LOCATION_SELLER) {
        
        //reset scout locations to current       
        going_front = b->location;
        going_back = b->location;
        am_here = b->location;

        //scouts for petrol station
        scout = check_distance(b, LOCATION_PETROL_STATION, "NULL",
                b->fuel);

        //is it possible to get to the petrol station with remaining fuel?
        if(b->fuel <= scout+(2*b->maximum_move)) {
            //not enough fuel left, check if cargo is empty first

            if(b->location->quantity > (determin_world_size(b))) {
            
                if(b->cargo == NULL) {
                    //low on fuel, but check how many turns left: do I need
                    //to refuel? (depending on if there's enough turns left
                    //to make it worth it)
                    
                    if(b->turns_left >= total_turns(b)) {

                        //check if I have enough money for fuel
                        if(b->cash > ((highest_price)*(b->fuel_tank_capacity)/2)) {
                            //yes I have money for petrol if I buy wares
                            *action = ACTION_BUY;
                            *n = how_much_buy(b, (highest_price)*(b->fuel_tank_capacity-b->fuel));
                            return;

                        } else {
                            //move for petrol instead because limited money
                            scout = check_distance(b, LOCATION_PETROL_STATION, "NULL",
                                     b->fuel);

                            *action = ACTION_MOVE;
                            *n = scout;
                        }

                    } else {
                        //check if there is a buyer within distance
                        scout = check_distance(b, LOCATION_BUYER,
                                b->location->commodity->name, b->fuel);

                        //is the buyer is too far away for me to get to within
                        //3 turns?
                        if(scout <= 3*(b->maximum_move)) {
                            //no there is a close buyer
                            *action = ACTION_BUY;
                            *n = max_buy(b);
                            return;

                        } else {
                            //it is too far. Bot will now do nothing and wait
                            //for game to end

                            *action = ACTION_MOVE;
                            *n = NOTHING;
                            return;
                        }
                    }

                } else {
                    //cargo already filled
                    //going to check if any buyer within walking distance
                    scout = check_distance(b, LOCATION_BUYER, 
                            b->cargo->commodity->name, b->fuel);
                    
                    //compare to petrol station location
                    scout2 = check_distance(b, LOCATION_PETROL_STATION, "NULL",
                             b->fuel);

                    if(abs(scout) <= abs(scout2) && scout*scout2 > 0) {
                        //if buyer is closer, go buyer
                        *action = ACTION_MOVE;
                        *n = scout;
                        return;

                    } else {
                        //if station is closer, go station
                        *action = ACTION_MOVE;
                        *n = scout2;
                        return;
                    }

                }
                    
            } else {
                //theres nothing here at the shop! and there's not much fuel left
                //see if there are any cargo
                if(b->cargo == NULL) {
                    //searching for a seller or a petrol station
                    //if both in the same direction, stop at whichever one is
                    //first, if the seller is further or in opposite direction
                    //to p.station, go to p.station
                    scout = check_distance(b, LOCATION_SELLER, "NULL", 
                            b->fuel);

                    //compare to petrol station location
                    scout2 = check_distance(b, LOCATION_PETROL_STATION, "NULL",
                             b->fuel);

                    if(abs(scout) <= abs(scout2) && scout*scout2 > 0) {
                        //if buyer is closer, go buyer
                        *action = ACTION_MOVE;
                        *n = scout;
                        return;
                        
                    } else {
                        //if station is closer, go station
                        *action = ACTION_MOVE;
                        *n = scout2;
                        return;
                    }

                } else {
                    //searching for a buyer or a petrol station
                    //if both in the same direction, stop at whichever one is
                    //first, if the buyer is further or in opposite direction
                    //to p.station, go to p.station
                    scout = check_distance(b, LOCATION_BUYER, 
                            b->cargo->commodity->name, b->fuel);
                    
                    //compare to petrol station location
                    scout2 = check_distance(b, LOCATION_PETROL_STATION, "NULL",
                             b->fuel);

                    if(abs(scout) <= abs(scout2) && scout*scout2 > 0) {
                        //if buyer is closer, go buyer
                        *action = ACTION_MOVE;
                        *n = scout;
                        return;
                        
                    } else {
                        //if station is closer, go station
                        *action = ACTION_MOVE;
                        *n = scout2;
                        return;
                    }

                }

            }

        } else {
            //is there stuff for bot to buy here there?
            if(b->location->quantity > (determin_world_size(b))) {
                //yes there are stock
                if(b->cargo == NULL) {
                    //buy from here because empty cargo
                    if(b->turns_left >= total_turns(b)) {
                        //more than certain amount of turns, 
                        //I need to save some money for petrol
                        *action = ACTION_BUY;
                        *n = how_much_buy(b, 
                             ((highest_price)*(b->fuel_tank_capacity-b->fuel)));

                    } else {
                        //There is less than total_turns(b) turns left
                        //check if there is a buyer (for this seller) within distance
                        scout = check_distance(b, LOCATION_BUYER,
                                b->location->commodity->name, b->fuel);

                        if(scout <= 3*(b->maximum_move)) {
                            //there is a buyer close
                            *action = ACTION_BUY;
                            *n = max_buy(b);
                            return;
                        } else {
                            //do nothing because no buyers are close
                            *action = ACTION_MOVE;
                            *n = NOTHING;
                            return;
                        }
                    }

                } else {
                    //cargo is filled, going to go to a seller
                    scout = check_distance(b, LOCATION_BUYER,
                            b->cargo->commodity->name, b->fuel);

                    *action = ACTION_MOVE;
                    *n = scout;
                }

            } else {
                //nothing for sale here
                if(b->cargo == NULL) {
                    //if cargo is empty, find a seller
                    scout = check_distance(b, LOCATION_SELLER,
                            "NULL", b->fuel);

                    *action = ACTION_MOVE;
                    *n = scout;

                } else {
                    //find a buyer for my cargo
                    scout = check_distance(b, LOCATION_BUYER,
                            b->cargo->commodity->name, b->fuel);

                    *action = ACTION_MOVE;
                    *n = scout;
                }
            }

        }

    //when the location is at buyer
    } else if(where_bot->type == LOCATION_BUYER) {
        going_front = b->location;
        going_back = b->location;

        if(b->location->quantity > determin_world_size(b)) {
            //has items it can buy

            if(b->cargo != NULL && b->cargo->quantity > 0
               && (strcmp(b->cargo->commodity->name, 
               b->location->commodity->name) == 0)) {
                //I have cargo, and it's the same as the buyer
                //so bot must sell
                *action = ACTION_SELL;
                *n = b->cargo->quantity;

            } else if(b->cargo == NULL || b->cargo->quantity == 0) {
                //nothing in cargo
                //check fuel
                if(b->fuel >= scout+(2*b->maximum_move)) {
                    //got enough fuel to find another seller to stock up
                    scout = check_distance(b, LOCATION_SELLER, "NULL",
                            b->fuel);

                    *action = ACTION_MOVE;
                    *n = scout;

                } else {
                    //not enough fuel
                    if(b->turns_left >= total_turns(b)) {
                        //see if there is a seller along the way of petrol
                        //station
                        scout = check_distance(b, LOCATION_SELLER, "NULL", 
                            b->fuel);

                        //compare to petrol station location
                        scout2 = check_distance(b, LOCATION_PETROL_STATION, "NULL",
                                 b->fuel);

                        if(abs(scout) <= abs(scout2) && scout*scout2 > 0) {
                            //if seller is closer, go buyer
                            *action = ACTION_MOVE;
                            *n = scout;
                            return;
                            
                        } else {
                            //if station is closer, go station
                            *action = ACTION_MOVE;
                            *n = scout2;
                            return;
                        }

                    } else {
                        //not enough turns left to do much, go to a seller and
                        //within the seller branch of choices it will know
                        //whether there is not enough turns to buy something
                        //and find a buyer before game ends
                        scout = check_distance(b, LOCATION_SELLER, 
                                "NULL", b->fuel);

                        *action = ACTION_MOVE;
                        *n = scout;

                    }

                }

            } else if(b->cargo != NULL && b->cargo->quantity > 0
                      && strcmp(b->cargo->commodity->name,
                      b->location->commodity->name) != 0) {
                //has things in cargo but not the same as buyer
                //check fuel

                if(b->fuel >= scout+(2*b->maximum_move)) {
                    //got enough fuel to find another buyer
                    scout = check_distance(b, LOCATION_BUYER, 
                            b->cargo->commodity->name, b->fuel);

                    *action = ACTION_MOVE;
                    *n = scout;

                } else {
                    //not enough fuel, compare buyer and p.station distance
                    scout = check_distance(b, LOCATION_BUYER, 
                            b->location->commodity->name, b->fuel);

                    //compare to petrol station location
                    scout2 = check_distance(b, LOCATION_PETROL_STATION, "NULL",
                             b->fuel);

                    if(abs(scout) <= abs(scout2) && scout*scout2 > 0) {
                        //if buyer is closer, go buyer
                        *action = ACTION_MOVE;
                        *n = scout;
                        return;
                        
                    } else {
                        //if station is closer, go station
                        *action = ACTION_MOVE;
                        *n = scout2;
                        return;
                    }

                }

            }

        } else {
            //does not have items the shop can buy
            //does bot have cargo?
            if(b->cargo != NULL) {
                //bot has cargo
                //see fuel

                if(b->fuel >= scout+(2*b->maximum_move)) {
                    //got enough fuel, find a buyer
                    scout = check_distance(b, LOCATION_BUYER, 
                            b->cargo->commodity->name, b->fuel);

                    *action = ACTION_MOVE;
                    *n = scout;

                } else {
                    //not enough fuel, compare distance of buyer to p.station
                    scout = check_distance(b, LOCATION_BUYER, 
                            b->cargo->commodity->name, b->fuel);

                    //compare to petrol station location
                    scout2 = check_distance(b, LOCATION_PETROL_STATION, "NULL",
                             b->fuel);

                    if(abs(scout) <= abs(scout2) && scout*scout2 > 0) {
                        //if buyer is closer, go buyer
                        *action = ACTION_MOVE;
                        *n = scout;
                        return;
                        
                    } else {
                        //if station is closer, go station
                        *action = ACTION_MOVE;
                        *n = scout2;
                        return;
                    }

                }

            } else {
                //bot has cargo
                if(b->fuel >= scout+(2*b->maximum_move)) {
                    //got enough fuel, go find seller
                    scout = check_distance(b, LOCATION_SELLER, 
                            "NULL", b->fuel);

                    *action = ACTION_MOVE;
                    *n = scout;

                } else {
                    //not enough fuel, check for p.station and sellers
                    scout = check_distance(b, LOCATION_SELLER, "NULL", 
                            b->fuel);

                    //compare to petrol station location
                    scout2 = check_distance(b, LOCATION_PETROL_STATION, "NULL",
                             b->fuel);

                    if(abs(scout) <= abs(scout2) && scout*scout2 > 0) {
                        //if buyer is closer, go buyer
                        *action = ACTION_MOVE;
                        *n = scout;
                        return;
                        
                    } else {
                        //if station is closer, go station
                        *action = ACTION_MOVE;
                        *n = scout2;
                        return;
                    }

                }

            }

        }


    //when the location is at petrol station
    } else if(where_bot->type == LOCATION_PETROL_STATION) {
        //is there fuel at the station?
        if(b->location->quantity > (determin_world_size(b)/2)) {
            //there is fuel
            if(b->turns_left >= total_turns(b)) {
                //fills up petrol if more than 7 turns remain
                if(b->fuel < b->fuel_tank_capacity) {
                    //fuel isn't full, so fuel up
                    *action = ACTION_BUY;
                    *n = (b->fuel_tank_capacity)-(b->fuel);
                } else {
                    //fuel is full
                    if(b->cargo != NULL) {
                        //find a buyer if necessary
                        scout = check_distance(b, LOCATION_BUYER, 
                                b->cargo->commodity->name, b->fuel);
                        *action = ACTION_MOVE;
                        *n = scout;
                    } else {
                        //find a seller if necessary
                        scout = check_distance(b, LOCATION_SELLER, 
                                "NULL", b->fuel);
                        *action = ACTION_MOVE;
                        *n = scout;
                    }
                }
                
            } else {
                //move away, there's not enough turns left to make purchasing
                //fuel worth it
                if(b->cargo != NULL) {
                    //there is cargo to be sold
                    scout = check_distance(b, LOCATION_BUYER, 
                        b->cargo->commodity->name, b->fuel);
                    *action = ACTION_MOVE;
                    *n = scout;
                } else {
                    //find a seller instead
                    scout = check_distance(b, LOCATION_SELLER, 
                        "NULL", b->fuel);
                    *action = ACTION_MOVE;
                    *n = scout;
                }
            }

        } else {
            //there's no fuel here, bot needs to move away to find a new petrol
            //station if necessary
            //this returns a new location with fuel, if there is one
            scout = check_distance(b, LOCATION_PETROL_STATION, "NULL", b->fuel);

            if(b->fuel <= scout+(2*b->maximum_move)) {
                //not enough fuel to ignore needing petrol
                if(b->turns_left >= total_turns(b)) {
                    //more than 5 turns: should fill up
                    if(b->cargo != NULL) {
                        //there's cargo
                        //see if there is buyer along the way to p.station
                        scout = check_distance(b, LOCATION_BUYER, 
                            b->cargo->commodity->name, b->fuel);

                        //compare to petrol station location
                        scout2 = check_distance(b, LOCATION_PETROL_STATION, "NULL",
                                 b->fuel);

                        if(abs(scout) <= abs(scout2) && scout*scout2 > 0) {
                            //if buyer is closer, go buyer
                            *action = ACTION_MOVE;
                            *n = scout;
                            return;
                            
                        } else {
                            //if station is closer, go station
                            *action = ACTION_MOVE;
                            *n = scout2;
                            return;
                        }
                    
                    } else {
                        //no cargo, see if there is a seller along the way
                        //to petrol
                        scout = check_distance(b, LOCATION_SELLER, "NULL", 
                            b->fuel);

                        //compare to petrol station location
                        scout2 = check_distance(b, LOCATION_PETROL_STATION, "NULL",
                                 b->fuel);

                        if(abs(scout) <= abs(scout2) && scout*scout2 > 0) {
                            //if seller is closer, go seller
                            *action = ACTION_MOVE;
                            *n = scout;
                            return;
                            
                        } else {
                            //if station is closer, go station
                            *action = ACTION_MOVE;
                            *n = scout2;
                            return;
                        }

                    }

                } else {
                    //not enough turns, see if has cargo
                    if(b->cargo != NULL) {
                        //go sell it
                        printf("Khajiit must sell the remainder of its wares.\n");
                        scout = check_distance(b, LOCATION_BUYER, 
                                b->cargo->commodity->name, b->fuel);

                        *action = ACTION_MOVE;
                        *n = scout;

                    } else {
                        //buy some more items if bot can
                        printf("Khajiit will buy some wares.\n");
                        scout = check_distance(b, LOCATION_SELLER, 
                                "NULL", b->fuel);

                        *action = ACTION_MOVE;
                        *n = scout;
                    }
                
                }

            } else {
                //enough fuel
                if(b->cargo != NULL) {
                    //go to sell
                    scout = check_distance(b, LOCATION_BUYER, 
                        b->cargo->commodity->name, b->fuel);
                    *action = ACTION_MOVE;
                    *n = scout;
                } else {
                    //go to seller to make further decisions
                    scout = check_distance(b, LOCATION_SELLER, 
                        "NULL", b->fuel);
                    *action = ACTION_MOVE;
                    *n = scout;
                }

            }

        }

    }

}

//works out a guess of the total turns based on how many bots there are
int total_turns(struct bot *b) {
    if(count_bot(b) > 5) {
        return 5;
    } else {
        return 0;
    }
}

//returns how much should be bought by bot
int how_much_buy(struct bot *b, int subtraction) {
    int buy_amount;

    //substraction represents how much need to be saved for fuel
    buy_amount = ((b->cash - subtraction)/b->location->price);
    return buy_amount;
}

//returns how many bots there are
int determin_world_size(struct bot *b) {
    return count_bot(b)-1;
}

//this function counts how many bots there are in the world
int count_bot(struct bot *b) {

    //pointers for location and incrementation
    struct location *temp = b->location;
    struct bot_list *temp_bot = temp->bots;

    int bot_count = 0;

    while (temp_bot != NULL) {
        bot_count++;
        temp_bot = temp_bot->next;
    }

    //keeps incrementing
    temp = temp->next;
    while (temp != b->location) {
        temp_bot = temp->bots;
        while (temp_bot != NULL) {
            bot_count++;
            temp_bot = temp_bot->next;
        }
        temp = temp->next;
    }

    //returns how many counted
    return bot_count;
}

//checks the distance from one place to another
int check_distance(struct bot *b, int type, char *commodity, int distance) {

    //counters
    int scout = 0;
    int size = (determin_world_size(b));

    //location scouts
    struct location *going_front = b->location;
    struct location *going_back = b->location;

    while (scout < distance) {
    //for(scout = 0; scout < distance; scout++) {

        if(type == LOCATION_PETROL_STATION) {
            //size ensures there is enough fuel to make it worth going there
            if(going_front->type == type && going_front->quantity > size/2) {
                return scout;
            }
            going_front = going_front->next;

            if(going_back->type == type && going_back->quantity > size/2) {
                //negative number scout to go backwards
                scout = -scout;
                return scout;
            }
            going_back = going_back->previous;

        
        } else if(type == LOCATION_BUYER) {    
            if(going_front->type == type && going_front->quantity > size) {
                if(strcmp(going_front->commodity->name, commodity) == 0) {
                    return scout;
                }
            }
            going_front = going_front->next;

            if(going_back->type == type && going_back->quantity > size) {
                if(strcmp(going_back->commodity->name, commodity) == 0) {
                    scout = -scout;
                    return scout;
                }
            }
            going_back = going_back->previous;

        } else if(type == LOCATION_SELLER) {
            if(going_front->type == type && going_front->quantity > size) {
                return scout;
            }
            going_front = going_front->next;

            if(going_back->type == type && going_back->quantity > size) {
                scout = -scout;
                return scout;
            }
            going_back = going_back->previous;
        }
        scout++;
    }
    return scout;
}

//function that gives me highest fuel price, this will be used to calculate
//how much I can buy of wares without running out money for fuel
int fuel_price(struct information *use, struct bot *b) {
 
    //most expensive fuel price
    int highest_price = 0;
    //simple counter
    int x = 0;

    int scout = 0;

    //counter for array
    struct location *fuel_price_search;

    //looping the map to record petrol prices!
    //this sets a starting point to begin the loop
    fuel_price_search = b->location->next;

    while(fuel_price_search != b->location) {
        if(fuel_price_search->type == LOCATION_PETROL_STATION) {
            use->station_price[x] = fuel_price_search->price;
            x++;
        }
        scout++;
        fuel_price_search = fuel_price_search->next;
    }

    //compare fuel prices
    int y = x-1;
    while(y >= 0) {
        if(use->station_price[x] > use->station_price[y]) {
            highest_price = use->station_price[x];
        } else {
            highest_price = use->station_price[y];
        }
        x--;
        y--;
    }

    return highest_price;
}

//function that returns how much I can buy
int max_buy(struct bot *b) {
    int max = 0;

    while(max > 0) {
        max = b->cash - b->location->price;
    }

    return max;
}
