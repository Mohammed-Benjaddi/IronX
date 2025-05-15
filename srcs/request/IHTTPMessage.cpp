#include "IHTTPMessage.hpp"

void IHTTPMessage::setStatusCode(int code)
{
    this->statusCode = code;
}

void IHTTPMessage::setStatusMessage(const std::string &message)
{
    this->statusMessage = message;
}

int IHTTPMessage::getStatusCode() const
{
    return statusCode;
}

std::string IHTTPMessage::getStatusMessage() const
{
    return statusMessage;
}