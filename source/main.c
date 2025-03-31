/**
 * TIC-TAC-TOE
 * 
 * 1. 2 players and 9 spaces
 * ** 9 sprites, once for each space, frame of space changes when clicked, only when 0 (unclaimed)
 * 2. players take turns, one sign per one field per turn, then turn changes
 * ** odd turns belong to one player, even turs to the other
 * 3. Players can only place on blank space
 * 4. A clears the playing field and resets the game
*/

#include <stdio.h>
#include <nds.h>
#include <nf_lib.h>
#include <filesystem.h>
#include <time.h>
#include "ObstacleList.h"

#define SCREEN_HEIGHT 192
#define SCREEN_WIDTH 256
#define STARMAN_HEIGHT 32
/*Sprite Data Strucutre to hold the id, pos, and frame of a sprite*/
typedef struct{
    u8 ID, X,Y,Frame;
} Sprite_Data;


Sprite_Data StarMan; //array to hold all the sprites
//Sprite_Data Obstacles[4];
int main(int argc, char **argv){

    consoleDemoInit(); //debug screen
    consoleDebugInit(DebugDevice_NOCASH);
    /*Initialize Screens and Graphics*/
    NF_Set2D(0,0); //set top screen
    NF_Set2D(1,0);

    nitroFSInit(NULL);
    NF_SetRootFolder("NITROFS");

    // Initialize OAM for the bottom screen
    //oamInit(&oamSub, SpriteMapping_1D_32, false);

    //initalize tile backgrounds
    NF_InitTiledBgBuffers();
    NF_InitTiledBgSys(0); //top screen
    NF_InitTiledBgSys(1); //bottom screen

    //Initialize sprites
    NF_InitSpriteBuffers();
    NF_InitSpriteSys(1); //initalize for bottom screen

    /*Load Background and Sprites*/

    //load Background into RAM
    NF_LoadTiledBg("bg/night_mounts", "bottomBG",256,256);
    //load sprites into RAM
    NF_LoadSpriteGfx("sp/star_guy_frame", 0,STARMAN_HEIGHT,STARMAN_HEIGHT); //load sprites for circle, cross, and blank
    NF_LoadSpritePal("sp/star_guy_frame",0); 
    NF_LoadSpriteGfx("sp/planets",1,STARMAN_HEIGHT,STARMAN_HEIGHT); //load sprites for circle, cross, and blank
    NF_LoadSpritePal("sp/planets",1);

    //Copy backround into VRAM
    NF_CreateTiledBg(1,0,"bottomBG");
    //copy sprites into VRAM for bottom screen
    NF_VramSpriteGfx(1,0,0,false); 
    NF_VramSpritePal(1,0,0);
    NF_VramSpriteGfx(1,1,1,false); 
    NF_VramSpritePal(1,1,1);
    
    u8 ID=5;
    int X=SCREEN_WIDTH/6;
    int Y=SCREEN_HEIGHT/2;
    NF_CreateSprite(1,ID,0,0,X,Y);  
    NF_SpriteFrame(1,ID,0); //first frame

    //Set up the sprite
    StarMan.ID=ID;
    StarMan.X=X;
    StarMan.Y=Y; //why am i doing it like this maybe i should just update starman directly instead of using Y
    StarMan.Frame=0; //frame is unclaimed

    /* Inital Obstacle Creation */
    ID = 0;
    int gap_y = STARMAN_HEIGHT * 2; // Initial gap is in the middle of the screen (2x height of player)
    int obstacleHeight = 32;

    // Create the first obstacle (top)
    struct Obstacle* firstObstacle = createObstacle(ID, SCREEN_WIDTH / 3, (SCREEN_HEIGHT / 2) - ((gap_y / 2) + obstacleHeight), 0);
    firstObstacle->next = firstObstacle; // Initialize circular linked list

    // Create the second obstacle (bottom)
    struct Obstacle* lastObstacle = insertEnd(firstObstacle, SCREEN_WIDTH / 3, (SCREEN_HEIGHT / 2) + (gap_y / 2), 0);

    // Create sprites for the obstacles
    NF_CreateSprite(1, firstObstacle->ID, 1, 1, firstObstacle->X, firstObstacle->Y); // Top obstacle
    NF_SpriteFrame(1,firstObstacle->ID,firstObstacle->Frame);
    NF_CreateSprite(1, lastObstacle->ID, 1, 1, lastObstacle->X, lastObstacle->Y);   // Bottom obstacle
    NF_SpriteFrame(1,lastObstacle->ID,lastObstacle->Frame);
    fprintf(stderr, "First Obstacle: ID=%d, X=%d, Y=%d\n", firstObstacle->ID, firstObstacle->X, firstObstacle->Y);
    fprintf(stderr, "Last Obstacle: ID=%d, X=%d, Y=%d\n", lastObstacle->ID, lastObstacle->X, lastObstacle->Y);
    int velocity= 0; //How fast, added to the position. If velocity<0 then flying. If >0 then falling
    const u8 acceleration=1; //added to the velocity every frame for fall
    int roundCounter=0;
    srand(time(NULL) + roundCounter);

    /*Per Frame*/
    while(1){
        scanKeys();
        
        /*Bounce Player up when A key is pressed*/
        if (KEY_A & keysDownRepeat())
        {
            velocity=-10;
            //fprintf(stderr, "Down Press\n");
        }
        else{ //if player is not bounced, they should fall
            velocity+=acceleration;
        }
        
        /*Get the new position of StarMax*/
        if (Y + velocity >= STARMAN_HEIGHT && Y + velocity <= SCREEN_HEIGHT-STARMAN_HEIGHT) { // Ensure Y stays within bounds
            Y += velocity;
        } else if (Y + velocity < STARMAN_HEIGHT) {
            Y = STARMAN_HEIGHT; // Clamp to top boundary
            velocity = 0;
        } else if (Y + velocity > SCREEN_HEIGHT-STARMAN_HEIGHT) {
            Y = SCREEN_HEIGHT-STARMAN_HEIGHT; // Clamp to bottom boundary
            velocity = 0;
        }
        
        /* make new obstacle if the current obstacle are 3/4 to end of screen*/
        
        /*if first obstacle in array are out of bounds, delete them from array*/
        
        /* move all obstacle by 3 px*/
        gap_y = rand() % ((STARMAN_HEIGHT*2) - (STARMAN_HEIGHT+4) + 1)+ (STARMAN_HEIGHT+4); //randomize the gap: starman height +8<gap<2*starman height
        struct Obstacle* current = firstObstacle;
        do {
            struct Obstacle* nextObstacle = current->next; // Save the next node before potential deletion
            fprintf(stderr, "Next OB ID: %d\n", nextObstacle->ID);
            current->X -= 3; // Move the obstacle
            //fprintf(stderr, "current ID: %d, X: %d\n", current->ID, current->X);
        
            if (current->X <= 0) { // If the obstacle is off the screen
                //fprintf(stderr, "DELETING obstacle ID: %d\n", current->ID);
                int temp_Y=0;
                NF_DeleteSprite(1, current->ID); // Delete the sprite
        
                // Determine the new obstacle's Y position
                if (current->ID % 2 == 0) {
                    temp_Y = (SCREEN_HEIGHT / 2) - ((gap_y / 2) + obstacleHeight);
                } else {
                    temp_Y = (SCREEN_HEIGHT / 2) + (gap_y / 2);
                }
        
                // Delete the current obstacle from the list
                //⚠️Should be deleteing the first 2 obstacles and creating 2 new ones
                firstObstacle = deleteObstacle(firstObstacle, current);
                
                // Create a new obstacle
                int temp_frame = rand() % (3 + 1);
                
                lastObstacle = insertEnd(lastObstacle, SCREEN_WIDTH, temp_Y, temp_frame);
                NF_CreateSprite(1, lastObstacle->ID, 1, 1, SCREEN_WIDTH, temp_Y);
                NF_SpriteFrame(1,lastObstacle->ID,lastObstacle->Frame);
               // fprintf(stderr, "Last obstacle ID: %d\n", lastObstacle->ID);
                roundCounter++;
            }else{
                NF_MoveSprite(1,current->ID, current->X, current->Y);
            }
        
            current = nextObstacle; // Move to the next obstacle
        
            //fprintf(stderr, "roundCounter: %d\n", roundCounter);
        } while (firstObstacle != NULL && current != firstObstacle);
        
        // fprintf(stderr, "position: %d\n\n", Y);

        /*Check for collision to End Game*/
        //if starmans (Xrange,Yrange) are in the range of either planet, then end game
        //⚠️⚠️⚠️TODO: currently only checking if theres a collision with top obstacle this
        // need to include bottom obstacle
        //should look like ((X collision)&&(Y top collision || YBottom Collision))
        if((StarMan.X+32>firstObstacle->X && StarMan.X<firstObstacle->X+32)
           // &&((StarMan.Y<firstObstacle->Y+32 && StarMan.Y+32>firstObstacle->Y)
            &&(StarMan.Y<firstObstacle->next->Y+32 && StarMan.Y+32>firstObstacle->next->Y)){
            fprintf(stderr, "TOP COLLISION: GAME END");
        }

        StarMan.Y=Y;
        NF_MoveSprite(1,StarMan.ID, X, StarMan.Y);
        StarMan.Frame = !StarMan.Frame;
        //fprintf(stderr, "frame: %d\n", StarMan.Frame);
        NF_SpriteFrame(1,StarMan.ID,StarMan.Frame);
        NF_SpriteOamSet(1); //update NFLib's sprite OAM system
        swiWaitForVBlank(); //wait for vertical blank
        oamUpdate(&oamSub); //update OAM of bottom screen engine
    }
    return 0;
 
}