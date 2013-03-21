#include "NeuralNode.h"


/**
*Not really a node, but a convenience method of defining a constant value input.
*/
NeuralNode::NeuralNode(float value)
{
	this->nrOfInputs=0;
	this->currentOutput=value;
}

NeuralNode::NeuralNode(float* pointer)
{
	this->function=SENSOR;
	this->nrOfInputs=1;
	this->currentOutput=*pointer;
	this->sensorInput=pointer;
}

NeuralNode::NeuralNode(int function, NeuralNode* input, float weight)
{
	this->function=function%NR_OF_FUNCTIONS;
	this->input1=input;
	this->input2=NULL;
	this->input3=NULL;
	this->weight1=weight;
	this->weight2=1;
	this->weight3=1;
	this->nrOfInputs=1;
}

NeuralNode::NeuralNode(int function, NeuralNode* input1, NeuralNode* input2, float weight1, float weight2)
{
	//printf("ENUM PROD %d, func: %d \n",PRODUCT,function);
	
	this->function=function%NR_OF_FUNCTIONS;
	this->input1=input1;
	this->input2=input2;
	this->input3=NULL;
	this->weight1=weight1;
	this->weight2=weight2;
	this->weight3=1;
	this->nrOfInputs=2;
}

NeuralNode::NeuralNode(int function, NeuralNode* input1, NeuralNode* input2, NeuralNode* input3, float weight1, float weight2, float weight3)
{
	this->function=function%NR_OF_FUNCTIONS;
	this->input1=input1;
	this->input2=input2;
	this->input3=input3;
	this->weight1=weight1;
	this->weight2=weight2;
	this->weight3=weight3;
	this->nrOfInputs=3;
}

float NeuralNode::getOutput()
{
	return this->currentOutput;
}

void NeuralNode::compute()
{
	if(nrOfInputs>0){
	switch(function){
		case SENSOR:
			currentOutput=*sensorInput;
			//printf("sensor out: %f",currentOutput);
			break;
		case SUM:
			currentOutput=sum();
			break;
		case PRODUCT:
			//printf("hej");
			currentOutput=product();
			break;
		case DIVIDE:
			currentOutput=divide();
			break;
		case SUMTHRESHOLD:
			currentOutput=sumThreshold();
			break;
		case GREATERTHAN:
			currentOutput=greaterThan();
			break;
		case SIGNOF:
			currentOutput=signOf();
			break;
		case MIN:
			currentOutput=myMin();
			break;
		case MAX:
			currentOutput=myMax();
			break;
		case ABS:
			currentOutput=abs();
			break;
		case IF:
			currentOutput=myIf();
			break;
		case INTERPOLATE:
			currentOutput=interpolate();
			break;
		case SIN:
			currentOutput=mySin();
			break;
		case COS:
			currentOutput=myCos();
			break;
		case ATAN:
			currentOutput=myAtan();
			break;
		case LOG:
			currentOutput=myLog();
			break;
		case EXPT:
			currentOutput=myExpt();
			break;
		case SIGMOID:
			currentOutput=sigmoid();
			break;
		default:
			perror("Undefined neural function");
			break;
		}
	}
}

inline float NeuralNode::sum(){
	if(input2 == NULL){input2=new NeuralNode((float)0);}
	/*printf("in1:%f in2:%f sum %f \n",input1->currentOutput,input2->currentOutput,
			input1->currentOutput * weight1
			+ 
			input2->currentOutput * weight2);*/
	return	input1->currentOutput * weight1
			+ 
			input2->currentOutput * weight2;
}

inline float NeuralNode::product(){
	if(input2 == NULL){input2=new NeuralNode((float)1);}
	/*printf("in1:%f in2:%f product %f \n",input1->currentOutput,input2->currentOutput,
			input1->currentOutput * weight1
			* 
			input2->currentOutput * weight2);*/
	return	input1->currentOutput * weight1
			* 
			input2->currentOutput * weight2;
}

inline float NeuralNode::divide(){
	if(input2 == NULL){input2=new NeuralNode((float)1);}
	return	input1->currentOutput * weight1 
			/ 
			input2->currentOutput * weight2;
}

/**
* Returns 1 if the weighted sum of inputs 1 & 2 are larger than the weighted treshold input (input 3).
* Returns 0 otherwise.
*/
inline float NeuralNode::sumThreshold(){
	if(input2 == NULL){input2=new NeuralNode((float)0);}
	if(input3 == NULL){input3=new NeuralNode((float)0);}
	return	(input1->currentOutput * weight1 + input2->currentOutput * weight2)
			>
			input3->currentOutput * weight3;
}

inline float NeuralNode::greaterThan(){
	if(input2 == NULL){input2=new NeuralNode((float)0);}
	return	input1->currentOutput*weight1
			>
			input2->currentOutput*weight2;
}

inline float NeuralNode::signOf(){
	float i = input1->currentOutput * weight1;
	if(i<0){return -1;}
	else return i>0;
}

inline float NeuralNode::myMin(){
	if(input2 == NULL){input2=new NeuralNode(std::numeric_limits<float>::max());}
	if(input3 == NULL){input3=new NeuralNode(std::numeric_limits<float>::max());}
	float temporaryMin = input1->currentOutput*weight1;
	if(input2->currentOutput*weight2<temporaryMin){temporaryMin=input2->currentOutput*weight2;}
	if(input3->currentOutput*weight3<temporaryMin){temporaryMin=input3->currentOutput*weight3;}
	return temporaryMin;
}

inline float NeuralNode::myMax(){
	if(input2 == NULL){input2=new NeuralNode((-1)*(std::numeric_limits<float>::max()));}
	if(input3 == NULL){input3=new NeuralNode((-1)*(std::numeric_limits<float>::max()));}
	float temporaryMax = input1->currentOutput*weight1;
	if(input2->currentOutput*weight2>temporaryMax){temporaryMax=input2->currentOutput*weight2;}
	if(input3->currentOutput*weight3>temporaryMax){temporaryMax=input3->currentOutput*weight3;}
	return temporaryMax;
}

inline float NeuralNode::abs(){
	float temp = input1->currentOutput*weight1;
	if(temp>=0){return temp;}
	else{return -temp;}
}

inline float NeuralNode::myIf(){
	if(input2 == NULL){input2=new NeuralNode((float)1);}
	if(input2->currentOutput*weight2){
		return input1->currentOutput*weight1;
	}
	else{return 0;}
}

inline float NeuralNode::interpolate(){
	float wi1 = input1->currentOutput*weight1;
	if(input2 == NULL){input2=new NeuralNode(wi1);}
	float wi2 = input2->currentOutput*weight2;
	if(input3 == NULL){input3=new NeuralNode(wi2);}
	float wi3 = input3->currentOutput*weight3;

	return (wi1+wi2+wi3)/3;
}

inline float NeuralNode::mySin(){
	printf("in1:%f sin %f \n",input1->currentOutput, sin(input1->currentOutput*weight1));
	return sin(input1->currentOutput*weight1);
}
	
inline float NeuralNode::myCos(){
	return cos(input1->currentOutput*weight1);
}

inline float NeuralNode::myAtan(){
	return atan(input1->currentOutput*weight1);
}

inline float NeuralNode::myLog(){
	return log(input1->currentOutput*weight1);
}

inline float NeuralNode::myExpt(){
	if(input2 == NULL){input2=new NeuralNode((float)1);}
	return	pow(
				input1->currentOutput*weight1 ,
				input2->currentOutput*weight2
			);
}

inline float NeuralNode::sigmoid(){
	float x = input1->currentOutput*weight1;
	return 1/(1+exp(-x)); 
}

/**
*Integration of the hyperbolic tangent, from weighted inputs 1 to 2
*/
/*inline float NeuralNode::integrate(){
	
}*/

//, differentiate, smooth, memory, oscillate-wave, and oscillate-saw


void NeuralNode::setWeights(float w1, float w2, float w3){

}

NeuralNode::~NeuralNode(void)
{
}
