
#include <cstdio>
#include "ChatClient.h"
#include "src/SecureStreaming/Codec.cpp"

using Demo::error_code;
using Demo::ChatClient;
using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

ChatClient::ChatClient(std::string serverAddress, KeyPairGenerator* keyPairGenerator, Codec* codec) :
        clientId(-1),
        serverAddress(serverAddress),
        keyPairGenerator(keyPairGenerator),
        codec(codec),
        running(false),
        clientThread(JoinableThread(voidAction)) {
    clientThread.safeExit();
    initializeClientCallbacks();
    clientThread = JoinableThread(&Client::run, &client);
    initializeChatOutputIfNeeded();
}

void ChatClient::initializeClientCallbacks() {
    try {
        client.clear_access_channels(websocketpp::log::alevel::all);
        client.clear_error_channels(websocketpp::log::alevel::all);

        client.init_asio();
        client.start_perpetual();

        client.set_open_handler(bind(&ChatClient::onOpen, this, _1));
        client.set_fail_handler(bind(&ChatClient::onFail, this, _1));
        client.set_close_handler(bind(&ChatClient::onClose, this, _1));
        client.set_message_handler(bind(&ChatClient::onMessage, this, _1, _2));
    } catch (websocketpp::exception& e) {
        std::cout << std::endl << e.what() << std::endl;
    } catch (std::runtime_error& e) {
        std::cout << std::endl << e.what() << std::endl;
    } catch (...) {
        std::cerr << std::endl << "Something went terribly wrong with the client... shutting down..." << std::endl;
    }
}

void ChatClient::initializeChatOutputIfNeeded() {
    if (!chatOutput.is_open()) {
        chatOutput = std::ofstream(OUTPUT_FILE_NAME, std::ios::app);
    }
}

void ChatClient::onOpen(ConnectionHandle handle) {
    connectionMetadata.status = OPEN_STATUS;
    auto connection = client.get_con_from_hdl(handle);
    connectionMetadata.server = connection->get_response_header("Server");
}

void ChatClient::onFail(ConnectionHandle handle) {
    connectionMetadata.status = FAIL_STATUS;
    auto connection = client.get_con_from_hdl(handle);
    connectionMetadata.server = connection->get_response_header("Server");
    connectionMetadata.errorReason = connection->get_ec().message();
}

void ChatClient::onClose(ConnectionHandle handle) {
    connectionMetadata.status = CLOSE_STATUS;
    auto connection = client.get_con_from_hdl(handle);
    std::stringstream s;
    s << "close code: " << connection->get_remote_close_code() << " ("
      << websocketpp::close::status::get_string(connection->get_remote_close_code())
      << "), close reason: " << connection->get_remote_close_reason();
    connectionMetadata.errorReason = s.str();
}

void ChatClient::onMessage(ConnectionHandle handle, MessagePointer messagePointer) {
    string messageContent = extractMessageContent(messagePointer);

    if (isMessageFromServer(messageContent)) {
        messageContent = messageContent.substr(Demo::SERVER_MESSAGE_PREFIX.length());
        processServerMessage(messageContent);
    } else if (isPublicKey(messageContent)) {
        saveNewSenderPublicKey(messageContent);
    } else {

        auto senderId = extractAndRemoveSenderId(messageContent);
        if (isMessageFromMe(senderId)) {
            initializeChatOutputIfNeeded();
            outputMessageInChatFrom(I_AM_THE_AUTHOR, clientId, lastSentMessage);
        } else {
            auto signature = extractAndRemoveSignature(messageContent);
            auto encryptedMessage = std::move(messageContent); // Change of name to improve code readability
            bool signatureIsValid = validateSignature(signature, encryptedMessage);

            if (signatureIsValid) {
                try{
                    auto messageToOutput = decryptMessage(encryptedMessage);
                    initializeChatOutputIfNeeded();
                    outputMessageInChatFrom(THEY_ARE_THE_AUTHOR, senderId, messageToOutput);

                } catch (...){
                    printErrorMessage(" impossible to decrypt message.");
                }
            } else {
                printErrorMessage(" invalid signature. Cannot confirm message authenticity.");
            }
        }
    }
}

bool ChatClient::validateSignature(string const& signature, string const& encryptedMessage) const {
    auto publicKey = keyPairGenerator->fetchPublicKey(OTHER_PUBLIC_KEY_FILENAME);
    auto signatureIsValid = codec->validateSignature(publicKey, encryptedMessage, signature);
    return signatureIsValid;
}

string ChatClient::decryptMessage(std::string const& encryptedMessage) const {
    auto privateKey = keyPairGenerator->fetchPrivateKey(PRIVATE_KEY_FILENAME);
    auto messageToOutput = codec->decrypt(privateKey, encryptedMessage);
    return messageToOutput;
}

void ChatClient::outputMessageInChatFrom(string const& author, int const& senderId, string const& messageToOutput) {
    chatOutput << author << "(" << senderId << ")" << OUTPUT_AUTHOR_SEPARATOR << messageToOutput << std::endl;
}

void ChatClient::start() {
    try {
        std::cout << "Connecting to: " << serverAddress << std::endl;
        error_code errorCode;
        auto connection = client.get_connection(serverAddress, errorCode);
        throwExceptionOnError("Could not create connection, ", errorCode);

        connectionHandle = connection->get_handle();

        client.connect(connection);

        std::cout << "Connected to: " << serverAddress << std::endl;
        running.store(true);

    } catch (websocketpp::exception& e) {
        std::cout << std::endl << e.what() << std::endl;
    } catch (std::exception& e) {
        std::cerr << std::endl << e.what() << std::endl;
    } catch (...) {
        std::cerr << std::endl << "Something went terribly wrong with the client... shutting down..." << std::endl;
    }

}

void ChatClient::processUserInput(std::string message) {
    if (message == "exit" || message == "quit" || message == "close") {
        stop();
    } else if (message == "sendkey") {
        sendPublicKey();
    } else {
        try{
            auto encryptedMessage = encryptMessage(message);
            auto messageToSend = signMessageAndAppendSignature(encryptedMessage);
            messageToSend = addSenderId(messageToSend);
            sendMessage(messageToSend);
            lastSentMessage = message;
        } catch (...){
            printErrorMessage(" Cannot send message");
        }
    }
}

bool ChatClient::isRunning() const {
    return running.load();
}

void ChatClient::safeClose() {
    client.stop();
    clientThread.safeExit();
}

void ChatClient::stop() {
    client.stop_perpetual();

    if (connectionMetadata.status == OPEN_STATUS) {
        std::cout << "> Closing connection " << connectionMetadata.id << std::endl;

        error_code errorCode;
        client.close(connectionMetadata.handle, websocketpp::close::status::going_away,
                     "Requesting connection close", errorCode);
        if (errorCode) {
            std::cout << "Error closing connection " << connectionMetadata.id
                      << ": " << errorCode.message() << std::endl;
        }
    }

    running.store(false);
    destroyChatOutput();
}

void ChatClient::destroyChatOutput() {
    if (chatOutput.is_open()) {
        chatOutput.close();
        std::remove(OUTPUT_FILE_NAME.c_str());
    }
}

void ChatClient::sendPublicKey() {
    auto publicKey = keyPairGenerator->fetchOwnPublicKeyAsString();
    std::string keyToSend = PUBLIC_KEY_PREFIX + publicKey;
    sendMessage(keyToSend);

}

string ChatClient::encryptMessage(string const& message) const {
    auto otherPublicKey = keyPairGenerator->fetchPublicKey(OTHER_PUBLIC_KEY_FILENAME);
    auto encryptedMessage = codec->encrypt(otherPublicKey, message);
    return encryptedMessage;
}

std::string ChatClient::signMessageAndAppendSignature(string const& encryptedMessage) const {
    auto privateKey = keyPairGenerator->fetchPrivateKey(PRIVATE_KEY_FILENAME);
    auto signature = codec->signMessage(privateKey, encryptedMessage);
    auto messageToSend = encryptedMessage + ENCRYPTED_MESSAGE_SIGNATURE_SEPARATOR + signature;
    return messageToSend;
}

std::string ChatClient::addSenderId(string const& message) const {
    auto messageToSend = std::to_string(clientId) + ID_MESSAGE_SEPARATOR + message;
    return messageToSend;
}

void ChatClient::sendMessage(std::string message) {
    error_code errorCode;
    client.send(connectionHandle, message, websocketpp::frame::opcode::text, errorCode);
    throwExceptionOnError("Error sending message: ", errorCode);
}

string ChatClient::extractMessageContent(Demo::MessagePointer const& message) const {
    string messageContent;
    if (message->get_opcode() == websocketpp::frame::opcode::text) {
        messageContent = message->get_payload();
    } else {
        messageContent = websocketpp::utility::to_hex(message->get_payload());
    }
    return messageContent;
}

bool ChatClient::isMessageFromServer(string const& messageContent) const {
    auto serverMessagePrefixPosition = messageContent.find(Demo::SERVER_MESSAGE_PREFIX);
    bool isServerMessage = serverMessagePrefixPosition != std::string::npos;
    return isServerMessage;
}

bool ChatClient::isPublicKey(string const& messageContent) const {
    auto publicKeyPrefixPosition = messageContent.find(PUBLIC_KEY_PREFIX);
    bool isPublicKey = publicKeyPrefixPosition != std::string::npos;
    return isPublicKey;
}

bool ChatClient::isMessageFromMe(int const& senderId) const {
    return senderId == clientId;
}

int ChatClient::extractAndRemoveSenderId(string& messageContent) const {
    auto idSeparatorPosition = messageContent.find(ID_MESSAGE_SEPARATOR);
    auto messageId = std::stoi(messageContent.substr(0, idSeparatorPosition));
    messageContent = messageContent.substr(idSeparatorPosition + ID_MESSAGE_SEPARATOR.length());
    return messageId;
}

std::string ChatClient::extractAndRemoveSignature(string& messageContent) const {
    auto signatureSeparatorPosition = messageContent.find(ENCRYPTED_MESSAGE_SIGNATURE_SEPARATOR);
    auto signature = messageContent.substr(signatureSeparatorPosition + ENCRYPTED_MESSAGE_SIGNATURE_SEPARATOR.length());
    messageContent = messageContent.substr(0, signatureSeparatorPosition);
    return signature;
}

void ChatClient::processServerMessage(string messageContent) {
    auto messagePrompt = Demo::SERVER_MESSAGE_ID_ASSIGNATION;
    assignNewIdIfRequired(messageContent, messagePrompt);
}

void ChatClient::assignNewIdIfRequired(string& messageContent, string const& messagePrompt) {
    auto prefixPosition = messageContent.find(messagePrompt);
    bool newIdAssignation = prefixPosition != std::string::npos;
    if (newIdAssignation) {
        auto index = prefixPosition + messagePrompt.length();
        messageContent = messageContent.substr(index);
        clientId = std::stoi(messageContent);
    }
}

void ChatClient::saveNewSenderPublicKey(string& messageContent) const {
    auto newPublicKey = messageContent.substr(PUBLIC_KEY_PREFIX.length());
    auto myPublicKey = keyPairGenerator->fetchOwnPublicKeyAsString();
    if (newPublicKey != myPublicKey) {
        keyPairGenerator->savePublicKeyAsStringFromSender(OTHER_PUBLIC_KEY_FILENAME, newPublicKey);
    }
}

void ChatClient::throwExceptionOnError(std::string message, error_code const& errorCode) {
    if (errorCode) {
        throw std::runtime_error(message + errorCode.message());
    }
}

void ChatClient::printErrorMessage(std::string errorMessage) const noexcept {
    std::cout << INTERNAL_CLIENT_ERROR_MESSAGE << errorMessage << std::endl;
}

