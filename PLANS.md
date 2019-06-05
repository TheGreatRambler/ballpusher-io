# Some Future Plans

- Run physics calculations on client by using a scheduling algorithm (probably weighted round robin) to assign a client a frame, then the locations of the objects are sent to the server which sends those locations to the clients which updates them
- Switch ReactPhysics3D for Urho3D all around, on the client and running headless on the server
- If the level is one person, keep physics on the client only

## Physics

- Rotation is completely locked (x,y,z)
- Hitbox is a 3D prism covering the player body
- When a step is taken, a ray is sent from `LOWEST_SLIDE_POINT` to `HIGHEST_WALK_POINT` at the location where the player would walk to determine the `COLLISION_Y`:
```
    DESTINATION_LOCATION            _ STUCK
         |                         |
    0    |  _ HIGHEST_WALK_POINT  <
   /|\   | |                       |_ WALK
    |    | |                       |
   / \ ___\|_ CURRENT_Y           <
    |      |                       |
    |      |                       |_ WALK
CURRENT_LOCATION                   |
           |_ LOWEST_WALK_POINT   <
           |                       |
           |                       |
           |                       |_ SLIDE
           |                       |
           |                       |
           |_ LOWEST_SLIDE_POINT  <
                                   |_ FALL
```
 - When slide is started, decrease friction of body.
 - Keep decreased friction until jump button is pressed, then, begin jump and revert friction value.
 - When fall is started:
```
 0
/|\
 |
/ \_ CURRENT_Y
 |
 |
 |
 |
 |
 |
 |
 |
 |_ LOWEST_RAYCAST_FALL_CHECK_POINT
```
- Every frame, cast ray down to `LOWEST_RAYCAST_FALL_CHECK_POINT`, if point is found. Go one more frame. Next frame, check again. If point found, continue to check for a point every frame. If not, set `PLAYER_Y` to the last set point and change animation to `LANDED`, changing the animation to `IDLE` after the animation is done.
- When jump activated, check collisions on each face:
  - 1 SIDE: Begin wall jump facing to the opposite side
  - 2 SIDE (adjacent): Begin wall jump at the opposite side of both sides combined together as a vector
  - 2 SIDE (not adjacent): Begin 2 wall jump (like a shimmy up the wall with legs on either side)
  - 3 SIDE: Begin wall jump at opposite of middle side
  - 4 SIDE: Begin 4 wall jump (yet to know what that means)
  - SIDE && FLOOR: Begin normal jump
  - FLOOR: Begin normal jump
  - CEILING && FLOOR: Do nothing
  - CEILING: Do nothing
- The faces are actually small cubes on each face:
  - THe `JUMP_COLLIDER` is for the jump algorithm explained above (ove caveat: for the floor and ceiling, the `JUMP_COLLIDER` covers the entire bottom like the touch collider below)
  - The `TOUCH_COLLIDER` is for determining whether you can walk forwards
  - Every face looks like this
  - The rectangle of the person hitbox is surrounded with these collision shapes (which do not actually effect the world, they are only there to determine collision)
  - The `JUMP_COLLIDER` is a cube so that a wall only in a corner can't be used to wall-jump against
```
               ______________________
              /|____________________/|
             //  _______           //
            //  |      /|         //
           //  /|     / | <-------------- JUMP_COLLIDER
          //  /_|____/  |       //
         //  |  |____|__|      //
        //   | /     | /      //
       //    |/      |/      //
      //     |_______|      //
     //                    //<----------- TOUCH_COLLIDER
    //                    //
   //                    //
  //                    //
 //                    //
/_____________________//
|_____________________|
```
- Restrict jumps per second to 0.66 (3 jumps per two seconds)

## Mechanics

- Lives in the game are sticks
- You get a life when you find a tree with a stick on the ground
- You attack enemies with a stick
- When you hit an enemy, including lava, there is a 9/10 chance you lose the stick. Sometimes, however, the stick is not lost
- If you do a normal jump when you land from a fall, the height of the jump is half of the fall height (with a minimum so you don't have a tiny jump when you fall from a short height) and the larger the fall, the greater the chance that the stick breaks
- Once all sticks are gone, no fancy moves can be undertaken and your health becomes the equivalent of three hearts. Once these are gone, you are dead
- You start with three sticks but can gain more sticks from trees
- If a tree is damaged, whether from the stick or from an enemy, it has a chance of dropping a stick, allowing for infinite lives if the player is aware of it. The tree breaks eventually, however, after a lot of damage
- When you die, coins are deducted from your supply to pay medical bills. If you don't have the 5 coins to pay for a doctor, you go to the beginning of the level/world (haven't figured it out yet)
- Stick particulars:
  - Sticks are shown in the top left and any stick can be chosen at any one time (scroll through items using a keybind I haven't figured out yet)
  - Powerups are special sticks that grant special abilities
  - They are sent to the end of the stick queue and they look the same except for a different color
  - They are automatically selected when picked up (along with any normal stick), but you can always manually choose the stick if need be (autoselect can be a setting)
  - Upon using a stick, the current speed is taken to account to add damage to the stick
  - The damage only effects the stick above a certain point
  - When the damage exceeds the health of a stick, it breaks
  - The stick can break upon a normal use or a high-speed use, both work
  - When broken, the stick gives extra veolocity to the player as opposed to its normal tension
  - The damage bar is always shown for each stick in the top left (see above)
  - For sticks, an example:
    - Brown: Normal
    - Green: Autoheal
    - Blue: Extra health
    - White: Floaty
    - Red: Extra damage to enemies and boost upwards when hitting water (steam pushes upwards) (like lava)
    - Yellow: Coins given on every use
    - ETC...
