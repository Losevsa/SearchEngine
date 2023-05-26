
#include "answer.h"

Answer::Answer()
{

}

void Answer::addRequests(const Answers& answer)
{
    this->answer.push_back(answer);
}

int Answer::getSizeOfAnswer()
{
    return answer.size();
}

const Answers &Answer::getAnswer(const int &index)
{
    return answer[index];
}

