#pragma once
#include <vector>
#include "State.h"
#include "../DbElement.h"

class Chromosome : public DbElement
{
private:
	std::vector<State> states;
	double fitness;
	int nextStateIndex;
public:
	Chromosome(void);
	Chromosome(std::vector<State> s);
	~Chromosome(void);
	int temp;
	State getNextState();
	void setFitness(double value);
	std::vector<State> getStates(void) const;
	void addState(State s);
	int getFitness(void) const;
};

