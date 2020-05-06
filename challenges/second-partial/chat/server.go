// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 254.
//!+

// Chat is a server that lets clients chat with each other.
package main

import (
	"bufio"
	"fmt"
	"log"
	"net"
	"os"
	"strconv"
	"strings"
	"time"
)

//Luis Wilson A00226649

var usage string = "\nUsage: go run server.go -host <address> -port <port>"

//!+broadcaster
//type client chan<- string

type chatClient struct {
	username   string
	address    string
	channel    chan string //chan<- string
	isAdmin    bool
	connection net.Conn
	isKicked   bool
	//timestamp?
}

type msgCmdBody struct {
	toUser     string
	message    string
	senderUser string
	senderCh   chan string
}

type userCmdBody struct {
	interestedIn string
	senderCh     chan string
}

type kickCmdBody struct {
	senderUsername string
	senderCh       chan string
	userToKick     string
}

var (
	entering     = make(chan *chatClient)
	leaving      = make(chan *chatClient)
	messages     = make(chan string)
	clientAccept = make(chan string)
	commands     = make(chan string)
	usersCmd     = make(chan chan string)
	msgCmd       = make(chan msgCmdBody)
	userCmd      = make(chan userCmdBody)
	kickCmd      = make(chan kickCmdBody)
	arrival      = make(chan string)
)

//var clients = make(map[chatClient]bool)

func broadcaster() {
	//chatClients := make(map[chatClient]bool)
	clients := make(map[*chatClient]bool) // all connected clients
	for {
		select {
		case msg := <-messages:
			for cli := range clients {
				cli.channel <- msg
			}

		case cli := <-entering:
			//check that username doesnt already exist
			isUnique := true
			isEmpty := true

			for c := range clients {
				isEmpty = false
				if c.username == cli.username {
					serverAnnounce(cli.channel, "Username already in use. Please enter to the chat with a different username.")
					close(cli.channel)
					isUnique = false
					break
				}
			}

			if isUnique {
				clients[cli] = true
				serverSelf("New connected user [" + cli.username + "]")
				serverAnnounce(cli.channel, "Welcome to the Simple IRC Server")
				serverAnnounce(cli.channel, "Your user ["+cli.username+"] is successfully logged")
				clientAccept <- "yes"
				if isEmpty {
					cli.isAdmin = true
					serverSelf("[" + cli.username + "] was promoted as the channel ADMIN")
					serverAnnounce(cli.channel, "Congrats, you were the first user.")
					serverAnnounce(cli.channel, "You're the new IRC Server ADMIN")
					/*
								for cc := range clients {
							if arr != cc.username {
								cc.channel <- "New connected user [" + arr + "]"
							}

						}
					*/

				}
			} else {
				serverSelf("Client " + cli.address + " failed to connect due to duplicate username.")
				clientAccept <- "no"
			}

		case cli := <-leaving:
			cli.connection.Close()
			delete(clients, cli)
			if cli.isKicked == false {

				for cc := range clients {
					serverAnnounce(cc.channel, "["+cli.username+"] left channel")
				}

				serverSelf("[" + cli.username + "] left")
				//conn.Close()
			}
			close(cli.channel)

			if cli.isAdmin {
				//make other user admin
				for possAdmin := range clients {
					possAdmin.isAdmin = true
					serverAnnounce(possAdmin.channel, "You're the new IRC Server ADMIN")
					serverSelf("[" + possAdmin.username + "] was promoted as the channel ADMIN")
					for nonAdmins := range clients {
						if nonAdmins.username != possAdmin.username {
							serverAnnounce(nonAdmins.channel, "["+possAdmin.username+"] was promoted as the channel ADMIN")
						}
					}
					break
				}

			}

		case clientCh := <-usersCmd:
			var connectedUsers string
			for cli := range clients {
				connectedUsers += cli.username + ", "
			}

			serverAnnounce(clientCh, connectedUsers)

		case msgC := <-msgCmd:
			var receivingCh chan string = nil

			for cli := range clients {
				if cli.username == msgC.toUser {
					receivingCh = cli.channel
					break
				}
			}

			if receivingCh != nil {
				userSay(receivingCh, msgC.senderUser, msgC.message)
			} else {
				serverAnnounce(msgC.senderCh, "User does not exist or is not connected.")
			}
		case uCmd := <-userCmd:

			var outputMsg string = ""

			for cli := range clients {
				if cli.username == uCmd.interestedIn {
					outputMsg = "username: " + cli.username + ", IP: " + cli.address
					break
				}
			}

			if outputMsg != "" {
				serverAnnounce(uCmd.senderCh, outputMsg)
			} else {
				serverAnnounce(uCmd.senderCh, "User does not exist or is not connected.")
			}

		case kCmd := <-kickCmd:

			requestingClientIsAdmin := false

			for k := range clients {
				if kCmd.senderUsername == k.username {
					requestingClientIsAdmin = k.isAdmin
				}
			}

			if requestingClientIsAdmin {
				var kickChan chan string = nil
				var kickUserName string
				var toBeKicked *chatClient

				for cli := range clients {
					if cli.username == kCmd.userToKick {
						toBeKicked = cli
						kickChan = cli.channel
						kickUserName = cli.username
						toBeKicked.isKicked = true
						break
					}
				}

				if kickChan != nil {

					serverAnnounce(kickChan, "You were kicked from this channel")
					serverAnnounce(kickChan, "Bad language is not allowed on this channel")
					toBeKicked.connection.Close()
					serverSelf("[" + kickUserName + "] was kicked from the channel for bad language policy violation")
					for cc := range clients {
						serverAnnounce(cc.channel, "["+kickUserName+"] was kicked from the channel for bad language policy violation")
					}

				} else {
					serverAnnounce(kCmd.senderCh, "User does not exist or is not connected.")
				}
			} else {
				serverAnnounce(kCmd.senderCh, "You don't have the permission to do that.")
			}

		case arr := <-arrival:
			for cc := range clients {
				if arr != cc.username {
					serverAnnounce(cc.channel, "New connected user ["+arr+"]")
				}

			}

		}

	}
}

//!-broadcaster

func parseInput(input string) ([]string, string) {
	body := make([]string, 2)
	trimmed := strings.Trim(input, " ")
	if trimmed != "" {
		if trimmed[0] == '/' {
			separated := strings.SplitN(trimmed, " ", 2)

			switch separated[0] {
			case "/users": //ignore everything else

				return body, "users"

			case "/msg": //fix error: /msg lol (index out of range)
				if len(separated) == 2 {
					separatedMsg := strings.SplitN(strings.Trim(separated[1], " "), " ", 2)
					if len(separatedMsg) == 2 {
						body[0] = separatedMsg[1]
						body[1] = separatedMsg[0]
						return body, "msg"
					}
					return body, "error"
				}

				return body, "error"

			case "/time": //ignore everything else
				zone, _ := time.Now().Zone()
				body[0] = "Local Time: " + zone + " " + time.Now().Format("15:04:05\n")
				return body, "time"

			case "/user": //ignore everything else beyond <username> //fix error: /msg lol (index out of range)
				if len(separated) == 2 {
					separatedUser := strings.SplitN(strings.Trim(separated[1], " "), " ", 2)
					body[1] = separatedUser[0]
					return body, "user"
				}
				return body, "error"

			case "/kick": //ignore everything else beyond <username> //fix error: /msg lol (index out of range)
				if len(separated) == 2 {
					separatedUser := strings.SplitN(strings.Trim(separated[1], " "), " ", 2)
					body[1] = separatedUser[0]
					return body, "kick"
				}
				return body, "error"

			default: //treat as normal message

				body[0] = trimmed
				return body, "normal"
			}
			return body, "normal"
		}

		body[0] = trimmed
		return body, "normal"

	}

	return body, "empty"

}

//!+handleConn
func handleConn(conn net.Conn) { //handling client connection

	buffer := make([]byte, 512)
	size, err := conn.Read(buffer)

	if err != nil {
		log.Fatal(err)
	}

	username := string(buffer[:size])
	address := conn.RemoteAddr().String()

	ch := make(chan string)
	go clientWriter(conn, ch)

	currentChatClient := chatClient{username: username, address: address, channel: ch, isAdmin: false, connection: conn, isKicked: false}
	clientPointer := &currentChatClient

	entering <- clientPointer

	accepted := <-clientAccept

	if accepted == "yes" {
		arrival <- username
		input := bufio.NewScanner(conn)
		for input.Scan() {
			body, typ := parseInput(input.Text())

			switch typ {
			case "users":
				usersCmd <- ch
				break
			case "msg":
				if username != body[1] {
					msgCmd <- msgCmdBody{toUser: body[1], message: body[0], senderUser: username, senderCh: ch}
				} else {
					serverAnnounce(ch, "Don't be ashamed of talking to yourself! Don't worry, this'll be our secret.")
				}

				break
			case "time":
				serverAnnounce(ch, body[0])
				break
			case "user":
				userCmd <- userCmdBody{senderCh: ch, interestedIn: body[1]}
				break
			case "kick":
				if username != body[1] {
					kickCmd <- kickCmdBody{senderCh: ch, userToKick: body[1], senderUsername: username}
				} else {
					serverAnnounce(ch, "Whoa, there! You can't kick yourself. Why not just leave?")
				}

				break
			case "normal":
				userSay(messages, username, body[0])
				break
			case "error":
				serverAnnounce(ch, "Incomplete command.")
				break
			}

		}

		leaving <- clientPointer
		/*if !currentChatClient.isKicked {
			serverAnnounce(messages, "["+username+"] left channel")
			serverSelf("[" + username + "] left")
			//conn.Close()
		}*/

	} else if accepted == "no" {
		conn.Close()
	}

}

//!-handleConn

func clientWriter(conn net.Conn, ch <-chan string) {
	for msg := range ch {
		fmt.Fprintln(conn, msg) // NOTE: ignoring network errors
	}
}

func serverSelf(message string) {
	fmt.Println("irc-server > " + message)
}

func serverAnnounce(ch chan string, message string) { //receive chatClient chan
	labeledMsg := "irc-server > " + message
	ch <- labeledMsg
}

func userSay(ch chan string, username string, message string) {
	labeledMsg := username + " > " + message
	ch <- labeledMsg
}

func forcedBroadcast(message string, channel chan string) {
	messages <- message
}

//!+main
func main() {

	configuration := os.Args[1:]

	if len(configuration) != 4 {
		fmt.Println("Invalid number of arguments.", usage)
	} else if configuration[0] != "-host" || configuration[2] != "-port" {
		fmt.Println("Unsupported flags.", usage)
	} else if _, err := strconv.Atoi(configuration[3]); err != nil {
		fmt.Println("Bad port number.")
	} else {

		listener, err := net.Listen("tcp", configuration[1]+":"+configuration[3])
		if err != nil {
			log.Fatal(err)
		}

		go broadcaster() //global!!

		serverSelf("Simple IRC Server started at " + configuration[1] + ":" + configuration[3])
		serverSelf("Ready for receiving new clients")

		for {
			conn, err := listener.Accept()
			if err != nil {
				log.Print(err)
				continue
			}
			go handleConn(conn)
		}
	}

}

//!-main
