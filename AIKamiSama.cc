#include "Player.hh"
  
using namespace std;
  
/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME KamiSama
  
  
struct PLAYER_NAME : public Player {
  
    /**
     * Factory: returns a new instance of this class.
     * Do not modify this function.
     */
    static Player* factory () {
        return new PLAYER_NAME;
    }
  
    //returns a vector with the id of the empty neighbours
    void get_Em_Neig (const vector<int>& neighbours, vector<int>& empty_neighbours){
      for(int i = 0; i < (int)neighbours.size(); ++i){
        if(vertex(neighbours[i]).wall == -1 and vertex(neighbours[i]).bike == -1) empty_neighbours.push_back(neighbours[i]);
      }
    }
  
    //returns the number of possible moves if you move to this_neighbour and the id
    int possible_moves(int this_neighbour){
      int count = 0;
      for(int i = 0; i < (int)vertex(this_neighbour).neighbours.size(); ++i){
        //if there is a bike in a vertex next to this neighbour, we avoid the neighbour in order to maximize supervivence.
        if(vertex(vertex(this_neighbour).neighbours[i]).bike != -1) --count;
        if(vertex(vertex(this_neighbour).neighbours[i]).wall == -1) ++count;
      }
      return count;
    }
      
    //returns the "id" of the vertex with more possible moves.
    int calculate_next_movement(const vector<int>& empty_neighbours, const Bike& my_bike, Movement& movement){
      int avaible_moves = 0;
      int next_vertex = empty_neighbours[0];
        
      for(int i = 0; i < (int)empty_neighbours.size(); ++i){
        int aux = possible_moves(empty_neighbours[i]);
          
        //if there is a neighbour with bonus Ghost, we pick the bonus ALWAYS!,
        if(vertex(empty_neighbours[i]).bonus == Ghost) return empty_neighbours[i];
          
        //if there is a neighbour with bonus non-ghost and possible moves, we pick the bonus ALWAYS.
        if(vertex(empty_neighbours[i]).bonus != None and aux > 0) return empty_neighbours[i];
        if(avaible_moves < aux){
          next_vertex = empty_neighbours[i];
          avaible_moves = aux;
        }
        //else if(avaible_moves == 0 and aux == 0) next_vertex = empty_neighbours[i];
        else if(avaible_moves == aux){
          //if the avaible moves for 2 neighbours are the same, we calculate the next movement of each neighbour
          //to see which is more viable!
            if(aux > possible_moves(next_vertex)) next_vertex = empty_neighbours[i];
        }
      }
      if(avaible_moves == 0 and my_bike.bonus == Ghost) movement.use_bonus = true;
      return next_vertex;
    }
      
    /* Play method.
     * This method will be invoked once per each round.
     * You have to read the board and place your actions
     * for this round.
     */
    virtual void play () {
        vector<int> my_bikes = bikes(me());
        for(int i = 0; i < (int)my_bikes.size(); ++i){
            const Bike& my_bike = bike(my_bikes[i]);
  
            //Dead? Skip. No Turbo? Skip.
            if(!my_bike.alive) continue;
            if(round() % 2 and my_bike.turbo_duration <= 0) continue;
  
            // Find all empty neighbours
            vector<int> neighbours = vertex(my_bike.vertex).neighbours;
            vector<int> empty_neighbours;
            get_Em_Neig(neighbours, empty_neighbours);
  
            //Next Movement
            Movement movement(my_bike.id); //empty by now
  
            //calculate "best" move
            if(empty_neighbours.size() > 0) movement.next_vertex = calculate_next_movement(empty_neighbours, my_bike, movement);
            //here we have 0 movements left, because empty_neighbours size is 0!
            else if(my_bike.bonus == Ghost){
                movement.next_vertex = calculate_next_movement(empty_neighbours, my_bike, movement);
                movement.use_bonus = true;
            }
  
            //we use the bonus, unless it is ghost.
            if(my_bike.bonus != None and my_bike.bonus != Ghost) movement.use_bonus = true;
  
            command(movement);
        }
    }
  
};
  
  
/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
