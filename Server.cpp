#include <iostream>
#include <string>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <cstring>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using boost::asio::ip::tcp;
using namespace std;
using ::testing::AtLeast;
using::testing::Return;
using::testing::_;

class Client
{
public:
    virtual char* send_message() = 0;
    virtual void read_message(string message, int length) = 0;
};

class MockClient : public Client
{
public:
    MOCK_METHOD(char*, send_message, (), (override));
    MOCK_METHOD(void, read_message, (string message, int length), (override));
};

/**
 * Creates the necessary functions to handle socket connections and read/write operations
 *
 */
class tcp_connection: public boost::enable_shared_from_this<tcp_connection>
{
private:
    tcp::socket socket_;
    enum {max_length = 1024};
    char data[7]{};
    string type;

public:
    typedef boost::shared_ptr<tcp_connection> pointer;

    explicit tcp_connection(boost::asio::io_context& io_context): socket_(io_context){}

    static pointer create(boost::asio::io_context& io_context) { return pointer(new tcp_connection(io_context)); }

    tcp::socket& socket() { return socket_; }

    string start() // Reads data sent from client socket asynchronously
    {
        /*
        socket_.async_read_some(boost::asio::buffer(data, max_length),
                                boost::bind(&tcp_connection::handle_read, shared_from_this(),
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred));


        StringReader();
        */
    }

    string StringReader(MockClient& cl) // Reads string received to another string and ends if it detects ','
    {
        string typeS;
        char* dat = cl.send_message();

        for (int i = 0; i < 8; i++) {
            if (*(dat+i) != ',') {
                typeS.push_back(*(dat+i));
            } else {
                break;
            }
        }
        return typeS;
    }

    int send(MockClient& cl, string message, int length) // Sends a string to the client
    {
        /*
        socket_.async_write_some(boost::asio::buffer(message, max_length),
                                 boost::bind(&tcp_connection::handle_write, shared_from_this(),
                                             boost::asio::placeholders::error,
                                             boost::asio::placeholders::bytes_transferred));
        */

        cl.read_message(message, length);
        return length;
    }

    void handle_read(const boost::system::error_code& err, size_t bytes_transferred)
    // Executes code after receiving message
    {
        if (!err) {}

        else {
            cerr << "error: " << err.message() << endl;
            socket_.close();
        }
    }

    string handle_write(bool err, size_t bytes_transferred)
    // Executes code after sending message
    {
        if (!err) {
            memset(data, 0, 7);
        } else {
            return "error: ";
        }
    }
};

/**
 *  A TCP server that allows asynchronous client connections and calls the necessary functions to handle their
 *  connections
 */
class tcp_server
{
private:
    boost::asio::io_context& io_context_;
    tcp::acceptor acceptor_;
    string types_array[2];

    int i = 0;
    int j = 0;

    void start_accept() // Creates sockets for incoming connections
    {
        tcp_connection::pointer new_connection = tcp_connection::create(io_context_);


        acceptor_.async_accept(new_connection->socket(),
                               boost::bind(&tcp_server::handle_accept, this,new_connection,
                                           boost::asio::placeholders::error));
    }

public:
    explicit tcp_server(boost::asio::io_context& io_context): io_context_(io_context),   // Constructor RUNS ONCE
                                                              acceptor_(io_context, tcp::endpoint(tcp::v4(), 1234))
    {
        cout << "server running" << endl;

        start_accept();
    }

    void handle_accept(tcp_connection::pointer new_connection, const boost::system::error_code& err)
    // Runs everytime it receives a message from client, and it handles receiving a pair of cards
    {
        if (!err) {
            cout << "Client connected" << endl;

            if (i < 1) {
                //types_array[j] = new_connection->start();
                i++;
                j++;
            } else {
                //types_array[j] = new_connection->start();
                cout << types_array[0] << endl;
                cout << types_array[1] << endl;
                bool randBool = types_array[0] == types_array[1];

                if (randBool == 0) {
                    //new_connection->send("false");
                } else {
                    //new_connection->send("true");
                }

                fill_n(types_array, 2, "");
                cout << randBool << endl;
                i = 0;
                j = 0;
            }
        }
        start_accept();
    }
};

TEST(ServerTest, readString)
{
    boost::asio::io_context io_context;
    MockClient mockClient;

    char typeArr[7] = {'b','u','c','k','e','t',','};

    EXPECT_CALL(mockClient, send_message())
    .Times(1)
    .WillOnce(Return(typeArr));

    tcp_connection server(io_context);
    EXPECT_EQ(server.StringReader(mockClient), "bucket");
}

TEST(ServerTest, readStringNoComma)
{
    boost::asio::io_context io_context;
    MockClient mockClient;

    char array[4] = {'s','u','n'};

    EXPECT_CALL(mockClient, send_message())
    .Times(1)
    .WillOnce(Return(array));

    tcp_connection server(io_context);
    EXPECT_EQ(server.StringReader(mockClient), "sun");
}

TEST(ServerTest, readEmptyMessage)
{
    boost::asio::io_context io_context;
    MockClient mockClient;

    char array[1] = "";

    EXPECT_CALL(mockClient, send_message())
    .Times(1)
    .WillOnce(Return(array));

    tcp_connection server(io_context);
    EXPECT_EQ(server.StringReader(mockClient), "");
}

TEST(ServerTest, writeData)
{
    boost::asio::io_context io_context;
    MockClient mockClient;

    string result = "true";

    EXPECT_CALL(mockClient, read_message(result, result.length()))
    .Times(1);

    tcp_connection server(io_context);
    EXPECT_EQ(server.send(mockClient, result, result.length()), result.length());
}

TEST(ServerTest, writeDataWrongSize)
{
    boost::asio::io_context io_context;
    MockClient mockClient;

    string result = "true";

    EXPECT_CALL(mockClient, read_message(result, result.length()))
            .Times(1);

    tcp_connection server(io_context);
    EXPECT_EQ(server.send(mockClient, result, result.length() - 1), result.length());
}

TEST(ServerTest, writeEmptyData)
{
    boost::asio::io_context io_context;
    MockClient mockClient;

    string result = "";

    EXPECT_CALL(mockClient, read_message(result, result.length()))
            .Times(1);

    tcp_connection server(io_context);
    EXPECT_EQ(server.send(mockClient, result, result.length()), result.length());
}

int main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}