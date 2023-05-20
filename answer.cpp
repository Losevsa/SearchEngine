
#include "answer.h"

Answer::Answer()
{

}

void Answer::addRequests(Answers answer)
{
    this->answer.push_back(answer);
}

int Answer::getSizeOfAnswer()
{
    return answer.size();
}

Answers *Answer::getAnswer(int index)
{
    return &answer[index];
}

