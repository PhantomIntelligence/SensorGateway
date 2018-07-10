#ifndef XAVIER_CHATCLIENT_H
#define XAVIER_CHATCLIENT_H

#include <websocketpp/config/asio_no_tls_client.hpp>

#include <websocketpp/client.hpp>
#include <spirit-sensor-gateway/common/TypeDefinition.h>
#include "src/SecureStreaming/KeyPairGenerator.cpp"

#include "src/SecureStreaming/Codec.hpp"
#include "demo/common/ChatConfigurations.hpp"

namespace Demo {
    std::string const OTHER_PUBLIC_KEY_FILENAME = "other.pub";
    std::string const PRIVATE_KEY_FILENAME = "private.pem";
    std::string const PUBLIC_KEY_FILENAME = "public.pub";

    using Client = websocketpp::client<websocketpp::config::asio_client>;
    using MessagePointer = Client::message_ptr;
    using ConnectionHandle = websocketpp::connection_hdl;
    using websocketpp::lib::error_code;
    using Streaming::KeyPairGenerator;
    using Streaming::Codec;
    using Streaming::Logger;
    using std::string;

    struct ConnectionMetadata {
        int id;
        ConnectionHandle handle;
        std::string status;
        std::string URI;
        std::string server;
        std::string errorReason;
    };

    class ChatClient {
        std::string const CONNECTING_STATUS = "Connecting";
        std::string const OPEN_STATUS = "Open";
        std::string const CLOSE_STATUS = "Close";
        std::string const FAIL_STATUS = "Failed";
        std::string const INTERNAL_CLIENT_ERROR_MESSAGE = "INTERNAL CLIENT ERROR:";

        std::string const I_AM_THE_AUTHOR = " Me  ";
        std::string const THEY_ARE_THE_AUTHOR = " Them";
        std::string const OUTPUT_AUTHOR_SEPARATOR = " >>> ";
        std::string const OUTPUT_FILE_NAME = "chat-output.txt";

        std::string const PUBLIC_KEY_PREFIX = "PUBLICKEY-";
        std::string const ID_MESSAGE_SEPARATOR = "~$%$%$~";
        std::string const ENCRYPTED_MESSAGE_SIGNATURE_SEPARATOR = "%~$%$~%";

    public:

        explicit ChatClient(std::string serverAddress, KeyPairGenerator* keyPairGenerator, Codec* codec);

        ~ChatClient() noexcept = default;

        ChatClient(ChatClient const& other) = delete;

        ChatClient(ChatClient&& other) = delete;

        ChatClient& operator=(ChatClient const& other)& = delete;

        ChatClient& operator=(ChatClient&& other)& = delete;

        void start();

        void processUserInput(std::string message);

        bool isRunning() const;

        void safeClose();

    private:
        void initializeClientCallbacks();

        void initializeChatOutputIfNeeded();

        void onOpen(ConnectionHandle handle);

        void onFail(ConnectionHandle handle);

        void onClose(ConnectionHandle handle);

        void onMessage(ConnectionHandle handle, MessagePointer message);

        void outputMessageInChatFrom(string const& author, int const& senderId, string const& messageToOutput);

        void stop();

        void destroyChatOutput();

        void sendPublicKey();

        string encryptMessage(string const& message) const;

        string signMessageAndAppendSignature(string const& encryptedMessage) const;

        string addSenderId(string const& message) const;

        void sendMessage(std::string message);

        string extractMessageContent(MessagePointer const& message) const;

        bool isMessageFromServer(string const& messageContent) const;

        bool isPublicKey(string const& messageContent) const;

        bool isMessageFromMe(int const& senderId) const;

        int extractAndRemoveSenderId(string& messageContent) const;

        string extractAndRemoveSignature(string& messageContent) const;

        bool validateSignature(string const& signature, string const& encryptedMessage) const;

        string decryptMessage(string const& encryptedMessage) const;

        void processServerMessage(string messageContent);

        void assignNewIdIfRequired(string& messageContent, string const& messagePrompt);

        void saveNewSenderPublicKey(string& messageContent) const;

        static void throwExceptionOnError(std::string message, error_code const& errorCode);

        void printErrorMessage(std::string errorMessage) const noexcept ;

        ConnectionMetadata connectionMetadata;

        std::string serverAddress;
        std::string lastSentMessage;

        int clientId;
        Logger* logger;

        KeyPairGenerator* keyPairGenerator;

        Codec* codec;

        Client client;

        ConnectionHandle connectionHandle;

        AtomicFlag running;
        JoinableThread clientThread;

        std::ofstream chatOutput;

    };


}

#endif //XAVIER_CHATCLIENT_H
