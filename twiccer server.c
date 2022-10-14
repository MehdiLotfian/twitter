#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>
#include <time.h>
#pragma comment(lib, "Ws2_32.lib")
#define PORT 12345

//Shoud be in file
typedef struct user User;
struct user {
	char * username;
	char * token;
	int * tweet_ids_viewed;
	int tweets_num_viewed;
};
User * users = NULL;
int users_counter = 0;

char * CheckCommand(char * command);

//Connct To Client
int Initialize();
void FromClient(int server_socket, char * message_from_client);

//Server And Client Translator
char * TranslateSignUp(char * command);
char * TranslateLogIn(char * command);
char * TranslateSendTweet(char * command);
char * TranslateRefresh(char * command);
char * TranslateLike(char * command);
char * TranslateComment(char * command);
char * TranslateSearch(char * command);
char * TranslateFollow(char * command);
char * TranslateUnFollow(char * command);
char * TranslateSetBio(char * command);
char * TranslateProfile(char * command);
char * TranslateChangePassword(char * command);
char * TranslateDeleteTweet(char * command);
char * TranslateReTweet(char * command);
char * TranslateLogOut(char * command);
char * TranslateAdvanceSearch(char * command);

//Process 
int SignUp(char * username, char * password);
int LogIn(char * username, char * password);
int SendTweet(char * tweet, char * token, char ** username);
char * Refresh(char * token, char ** username);
char * Search(char * user_to_search, char * token, char ** username);
int Like(char * tweet_id, char * token, char ** username);
int Comment(char * tweet_id, char * comment, char * token, char ** username);
int Follow(char * user_to_follow, char * token, char ** username);
int UnFollow(char * user_to_unfollow, char * token, char ** username);
char * Profile(char * token, char ** username);
int SetBio(char * bio, char * token, char ** username);
int ChangePassword(char * current_password, char * new_password, char * token, char ** username);
int LogOut(char * token, char ** username);
char * AdvanceSearch(char * user_to_search, char * token, char ** username);
char * SearchTweet(char * word, char * token, char ** username);


void MakeUserFile(char * username, char * password, char * bio, char * followings, char * followers, char * personal_tweet);
void MakeUserStruct(char * username);
char * MakeToken(char * username);
int SearchUsersViaUserName(char * username);
int IsTokenUnic(char * token);
char * FindUserViaToken(char * token);
void MakeTweetFile(int tweet_id, char * username, char * tweet, char * comment, int likes);
int IsAuthorFollowed(char * author, char * followings);
int HasTweetReported(int tweet_id, char * username);
char * TranslateSearchTweet(char * command);

char RandomCharGenerator();
int FileExists(const char * filename);
int FindFileSize(FILE * file);

//Parsing Users
char * ParseUsername(char * file_name);
char * ParsePassword(char * file_name);
char * ParseBio(char * file_name);
char * ParseFollowings(char * file_name);
char * ParseFollowers(char * file_name);
char * ParsePersonalTweets(char * file_name);
//Parsing Tweets
int ParseTweetId(char * file_name);
char * ParseAuthor(char * file_name);
char * ParseContent(char * file_name);
char * ParseComments(char * file_name);
int ParseLikes(char * file_name);
int NumberOfFollowers(char * file_name);
int NumberOfFollowings(char * file_name);
char * FollowStatus(char * username, char * file_name);


int main() {
	char command[10000];
	int server_socket = Initialize();
	FromClient(server_socket, command);
}


char * CheckCommand(char * command) {
	char * respond;
	if (strncmp(command, "signup ", 7) == 0)
		respond = TranslateSignUp(command);
	else if (strncmp(command, "login ", 6) == 0)
		respond = TranslateLogIn(command);
	else if (strncmp(command, "send tweet", 10) == 0)
		respond = TranslateSendTweet(command);
	else if (strncmp(command, "refresh", 7) == 0)
		respond = TranslateRefresh(command);
	else if (strncmp(command, "like", 4) == 0)
		respond = TranslateLike(command);
	else if (strncmp(command, "comment", 7) == 0)
		respond = TranslateComment(command);
	else if (strncmp(command, "search", 6) == 0)
		respond = TranslateSearch(command);
	else if (strncmp(command, "follow", 6) == 0)
		respond = TranslateFollow(command);
	else if (strncmp(command, "unfollow", 8) == 0)
		respond = TranslateUnFollow(command);
	else if (strncmp(command, "set bio", 7) == 0)
		respond = TranslateSetBio(command);
	else if (strncmp(command, "profile", 7) == 0)
		respond = TranslateProfile(command);
	else if (strncmp(command, "change password", 15) == 0)
		respond = TranslateChangePassword(command);
	else if (strncmp(command, "delete tweet", 12) == 0)
		respond = TranslateDeleteTweet(command);
	else if (strncmp(command, "retweet", 7) == 0)
		respond = TranslateReTweet(command);
	else if (strncmp(command, "advance search", 14) == 0)
		respond = TranslateAdvanceSearch(command);
	else if (strncmp(command, "tweet search", 12) == 0)
		respond = TranslateSearchTweet(command);
	else if (strncmp(command, "logout", 6) == 0)
		respond = TranslateLogOut(command);
	else {
		printf("Bad request format.\n");
		respond = "{\"type\":\"Error\",\"message\":\"Bad Request format.\"}\n";
	}
	return respond;
}

//Connct To Client
int Initialize() {
	printf("Initializing...\n");
	WSADATA wsadata;
	int wsa_respond = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (wsa_respond != 0) {
		printf("Failed to run wsadata\n");
		return INVALID_SOCKET;
	}

	int server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket == INVALID_SOCKET) {
		printf("Could not  ceate socket\n");
		return INVALID_SOCKET;
	}

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(PORT);

	int bind_respond = bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if (bind_respond == SOCKET_ERROR) {
		printf("bind faild\n");
		return INVALID_SOCKET;
	}

	listen(server_socket, SOMAXCONN);
	printf("Initialized...\n");
	return server_socket;
}
void FromClient(int server_socket, char * message_from_client) {
	int length = sizeof(struct sockaddr_in);
	int client_socket;
	struct sockaddr_in client_addr;
	printf("Listening on port %d...\n", PORT);
	while ((client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &length)) != INVALID_SOCKET) {
		printf("Client connected...\n");
		memset(message_from_client, 0, 1000);
		recv(client_socket, message_from_client, 999, 0);
		printf("Request from /127.0.0.1: %s", message_from_client);
		char * respond = CheckCommand(message_from_client);
		printf("Response sent: %s", respond);
		send(client_socket, respond, strlen(respond), 0);
		closesocket(client_socket);
	}
}

//Server And Client Translator
char * TranslateSignUp(char * command) {
	char username[50], password[50];
	int command_counter = 7, username_counter = 0, password_counter = 0;
	while (strncmp(&command[command_counter], ", ", 2) != 0) {
		username[username_counter] = command[command_counter];
		username_counter++;
		command_counter++;
	}
	username[username_counter] = '\0';
	command_counter += 2;
	while (command[command_counter] != '\n') {
		password[password_counter] = command[command_counter];
		password_counter++;
		command_counter++;
	}
	password[password_counter] = '\0';
	int status = SignUp(username, password);
	char * respond = NULL;
	if (status == 1) {
		respond = "{\"type\":\"Successful\",\"message\":\"\"}\n";
		printf("User %s successfully signed up.\n", username);
	}
	else if (status == 0) {
		respond = "{\"type\":\"Error\",\"message\":\"This username is already taken\"}\n";
		printf("This username is already taken.\n");
	}
	return respond;
}
char * TranslateLogIn(char * command) {
	char username[50], password[50];
	int command_counter = 6, username_counter = 0, password_counter = 0;
	while (strncmp(&command[command_counter], ", ", 2) != 0) {
		username[username_counter] = command[command_counter];
		username_counter++;
		command_counter++;
	}
	username[username_counter] = '\0';
	command_counter += 2;
	while (command[command_counter] != '\n') {
		password[password_counter] = command[command_counter];
		password_counter++;
		command_counter++;
	}
	password[password_counter] = '\0';
	int status = LogIn(username, password);
	char * respond = NULL;
	if (status == 0) {
		printf("This username is not valid.\n");
		respond = "{\"type\":\"Error\",\"message\":\"This username is not valid.\"}\n";
	}
	else if (status == 1) {
		printf("Password is incorrect.\n");
		respond = "{\"type\":\"Error\",\"message\":\"Incorrect password.\"}\n";
	}
	else if (status == 2) {
		MakeUserStruct(username);
		char * token = MakeToken(username);
		printf("User %s successfully loged in.\n", username);
		respond = (char *)malloc(sizeof(char) * 64);
		sprintf(respond, "{\"type\":\"Token\",\"message\":\"%s\"}\n", token);
	}
	return respond;
}
char * TranslateSendTweet(char * command) {
	char token[33], tweet[3000];
	int command_counter = 11, token_counter = 0, tweet_counter = 0;
	while (strncmp(&command[command_counter], ", ", 2) != 0) {
		token[token_counter] = command[command_counter];
		token_counter++;
		command_counter++;
	}
	token[token_counter] = '\0';
	command_counter += 2;
	while (command[command_counter] != '\n') {
		tweet[tweet_counter] = command[command_counter];
		tweet_counter++;
		command_counter++;
	}
	tweet[tweet_counter] = '\0';
	char * username = NULL;
	int status = SendTweet(tweet, token, &username);
	char * respond = NULL;
	if (status == 1) {
		printf("User %s successfully sent a tweet.\n", username);
		respond = "{\"type\":\"Successful\",\"message\":\"Tweet is sent successfully.\"}\n";
	}
	else
		printf("Error sending tweet.\n");
	return respond;
}
char * TranslateRefresh(char * command) {
	char token[33];
	int command_counter = 8, token_counter = 0;
	while (command[command_counter] != '\n') {
		token[token_counter] = command[command_counter];
		token_counter++;
		command_counter++;
	}
	token[token_counter] = '\0';
	char * username = NULL;
	char * status = Refresh(token, &username);
	printf("User %s succesfully refreshed timeline.\n", username);
	char * respond = (char *)malloc(sizeof(char) * (35 + strlen(status)));
	sprintf(respond, "{\"type\":\"Successful\",\"message\":[%s]}", status);
	return respond;
}
char * TranslateLike(char * command) {
	char token[33], tweet_id[10];
	int command_counter = 5, token_counter = 0, tweet_id_counter = 0;
	while (strncmp(&command[command_counter], ", ", 2) != 0) {
		token[token_counter] = command[command_counter];
		token_counter++;
		command_counter++;
	}
	token[token_counter] = '\0';
	command_counter += 2;
	while (command[command_counter] != '\n') {
		tweet_id[tweet_id_counter] = command[command_counter];
		tweet_id_counter++;
		command_counter++;
	}
	tweet_id[tweet_id_counter] = '\0';
	char * username = NULL;
	int status = Like(tweet_id, token, &username);
	char * respond = NULL;
	if (status == 0) {
		printf("Tweet with this id is not available.\n");
		respond = "{\"type\":\"Error\",\"message\":\"Tweet with this id is not available.\"}\n";
	}
	else if (status == 1) {
		respond = (char *)malloc(sizeof(char) * 75);
		printf("User %s successfully liked tweet %s.\n", username, tweet_id);
		sprintf(respond, "{\"type\":\"Successful\",\"message\":\"Tweet %s successfully liked.\"}\n", tweet_id);
	}
	return respond;
}
char * TranslateComment(char * command) {
	char token[33], tweet_id[10], comment[300];
	int command_counter = 8, token_counter = 0, tweet_id_counter = 0, comment_counter = 0;
	while (strncmp(&command[command_counter], ", ", 2) != 0) {
		token[token_counter] = command[command_counter];
		token_counter++;
		command_counter++;
	}
	token[token_counter] = '\0';
	command_counter += 2;
	while (strncmp(&command[command_counter], ", ", 2) != 0) {
		tweet_id[tweet_id_counter] = command[command_counter];
		tweet_id_counter++;
		command_counter++;
	}
	tweet_id[tweet_id_counter] = '\0';
	command_counter += 2;
	while (command[command_counter] != '\n') {
		comment[comment_counter] = command[command_counter];
		command_counter++;
		comment_counter++;
	}
	comment[comment_counter] = '\0';
	char * username = NULL;
	int status = Comment(tweet_id, comment, token, &username);
	char * respond = NULL;
	if (status == 0) {
		printf("Tweet with this id is not available.\n");
		respond = "{\"type\":\"Error\",\"message\":\"Tweet with this id is not available.\"}\n";
	}
	else if (status == 1) {
		printf("User %s successfully sent a comment.\n", username);
		respond = "{\"type\":\"Successful\",\"message\":\"Comment is sent successfully.\"}\n";
	}
	return respond;
}
char * TranslateSearch(char * command) {
	char token[33], username_to_search[50];
	int command_counter = 7, token_counter = 0, username_to_search_counter = 0;
	while (strncmp(&command[command_counter], ", ", 2) != 0) {
		token[token_counter] = command[command_counter];
		token_counter++;
		command_counter++;
	}
	token[32] = '\0';
	command_counter += 2;
	while (command[command_counter] != '\n') {
		username_to_search[username_to_search_counter] = command[command_counter];
		username_to_search_counter++;
		command_counter++;
	}
	username_to_search[username_to_search_counter] = '\0';
	char * username = NULL;
	char * status = Search(username_to_search, token, &username);
	char * respond = NULL;
	if (status == NULL) {
		printf("This username is not valid.\n");
		respond = "{\"type\":\"Error\",\"message\":\"This username is not valid.\"}\n";
	}
	else {
		respond = (char *)malloc(sizeof(char) * (strlen(status) + 35));
		printf("User %s successfully searched user %s.\n", username, username_to_search);
		sprintf(respond, "{\"type\":\"Profile\",\"message\":%s}\n", status);
	}
	return respond;
}
char * TranslateFollow(char * command) {
	char token[33], username_to_follow[50];
	int command_counter = 7, token_counter = 0, username_to_follow_counter = 0;
	while (strncmp(&command[command_counter], ", ", 2) != 0) {
		token[token_counter] = command[command_counter];
		token_counter++;
		command_counter++;
	}
	token[token_counter] = '\0';
	command_counter += 2;
	while (command[command_counter] != '\n') {
		username_to_follow[username_to_follow_counter] = command[command_counter];
		username_to_follow_counter++;
		command_counter++;
	}
	username_to_follow[username_to_follow_counter] = '\0';
	char * username = NULL;
	int status = Follow(username_to_follow, token, &username);
	char * respond = NULL;
	if (status == 1) {
		respond = (char *)malloc(sizeof(char) * 200);
		printf("User %s successfully followed user %s.\n", username, username_to_follow);
		sprintf(respond, "{\"type\":\"Successful\",\"message\":\"User %s successfully followed user %s.\"}\n", username, username_to_follow);
	}
	else
		printf("Error following.\n");
	return respond;
}
char * TranslateUnFollow(char * command) {
	char token[33], username_to_unfollow[50];
	int command_counter = 9, token_counter = 0, username_to_unfollow_counter = 0;
	while (strncmp(&command[command_counter], ", ", 2) != 0) {
		token[token_counter] = command[command_counter];
		token_counter++;
		command_counter++;
	}
	token[token_counter] = '\0';
	command_counter += 2;
	while (command[command_counter] != '\n') {
		username_to_unfollow[username_to_unfollow_counter] = command[command_counter];
		username_to_unfollow_counter++;
		command_counter++;
	}
	username_to_unfollow[username_to_unfollow_counter] = '\0';
	char * username = NULL;
	int status = UnFollow(username_to_unfollow, token, &username);
	char * respond = NULL;
	if (status == 1) {
		respond = (char *)malloc(sizeof(char) * 200);
		printf("User %s successfully unfollowed user %s.\n", username, username_to_unfollow);
		sprintf(respond, "{\"type\":\"Successful\",\"message\":\"User %s successfully unfollowed user %s.\"}\n", username, username_to_unfollow);
	}
	else
		printf("Error unfollowing.\n");
	return respond;
}
char * TranslateSetBio(char * command) {
	char token[33], bio[300];
	int command_counter = 8, token_counter = 0, bio_counter = 0;
	while (strncmp(&command[command_counter], ", ", 2) != 0) {
		token[token_counter] = command[command_counter];
		token_counter++;
		command_counter++;
	}
	token[token_counter] = '\0';
	command_counter += 2;
	while (command[command_counter] != '\n') {
		bio[bio_counter] = command[command_counter];
		bio_counter++;
		command_counter++;
	}
	bio[bio_counter] = '\0';
	char * username = NULL;
	int status = SetBio(bio, token, &username);
	char * respond = NULL;
	if (status == 1) {
		printf("User %s successfully updated bio.\n", username);
		respond = "{\"type\":\"Successful\",\"message\":\"Bio is updated successfully.\"}\n";
	}
	else
		printf("Error setting bio.\n");
	return respond;
}
char * TranslateProfile(char * command) {
	char token[33];
	int command_counter = 8, token_counter = 0;
	while (command[command_counter] != '\n') {
		token[token_counter] = command[command_counter];
		token_counter++;
		command_counter++;
	}
	token[token_counter] = '\0';
	char * username = NULL;
	char * status = Profile(token, &username);
	char * respond = NULL;
	if (status != NULL) {
		respond = (char *)malloc(sizeof(char) * (strlen(status) + 35));
		printf("User %s successfully viewd his/her profile.\n", username);
		sprintf(respond, "{\"type\":\"Profile\",\"message\":%s}\n", status);
	}
	else
		printf("Error showing profile.\n");
	return respond;
}
char * TranslateChangePassword(char * command) {
	char token[33], current_password[50], new_password[50];
	int command_counter = 16, token_counter = 0, current_password_counter = 0, new_password_counter = 0;
	while (strncmp(&command[command_counter], ", ", 2) != 0) {
		token[token_counter] = command[command_counter];
		token_counter++;
		command_counter++;
	}
	token[token_counter] = '\0';
	command_counter += 2;
	while (strncmp(&command[command_counter], ", ", 2) != 0) {
		current_password[current_password_counter] = command[command_counter];
		current_password_counter++;
		command_counter++;
	}
	current_password[current_password_counter] = '\0';
	command_counter += 2;
	while (command[command_counter] != '\n') {
		new_password[new_password_counter] = command[command_counter];
		command_counter++;
		new_password_counter++;
	}
	new_password[new_password_counter] = '\0';
	char * username = NULL;
	int status = ChangePassword(current_password, new_password, token, &username);
	char * respond = NULL;
	if (status == 0) {
		printf("Entered current password is wrong.\n");
		respond = "{\"type\":\"Error\",\"message\":\"Entered current password is wrong.\"}\n";
	}
	else if (status == 1) {
		printf("User %s successfully changed his/her password", username);
		respond = "{\"type\":\"Successful\",\"message\":\"Password is changed successfully.\"}\n";
	}
	return respond;
}
char * TranslateLogOut(char * command) {
	char token[33];
	int command_counter = 7, token_counter = 0;
	while (command[command_counter] != '\n') {
		token[token_counter] = command[command_counter];
		token_counter++;
		command_counter++;
	}
	token[token_counter] = '\0';
	char * username = NULL;
	int status = LogOut(token, &username);
	char * respond = NULL;
	if (status == 1) {
		printf("User %s successfully loged out.\n", username);
		respond = "{\"type\":\"Successful\",\"message\":\"User loged out successfully.\"}\n";
	}
	else
		printf("Error logging out.\n");
	return respond;
}
char * TranslateDeleteTweet(char * command) {
	char token[33], tweet_id[10];
	int command_counter = 13, token_counter = 0, tweet_id_counter = 0;
	while (strncmp(&command[command_counter], ", ", 2) != 0) {
		token[token_counter] = command[command_counter];
		token_counter++;
		command_counter++;
	}
	token[token_counter] = '\0';
	command_counter += 2;
	while (command[command_counter] != '\n') {
		tweet_id[tweet_id_counter] = command[command_counter];
		tweet_id_counter++;
		command_counter++;
	}
	tweet_id[tweet_id_counter] = '\0';
	char * username = NULL;
	int status = DeleteTweet(tweet_id, token, &username);
	char * respond = NULL;
	if (status == 0) {
		printf("Tweet with this id is not available.\n");
		respond = "{\"type\":\"Error\",\"message\":\"Tweet with this id is not available.\"}\n";
	}
	else if (status == 1) {
		printf("Tweet blongs to someone else.\n");
		respond = "{\"type\":\"Error\",\"message\":\"Tweet blongs to someone else.\"}\n";
	}
	else if (status == 2) {
		respond = (char *)malloc(sizeof(char) * 75);
		printf("User %s successfully deleted tweet %s.\n", username, tweet_id);
		sprintf(respond, "{\"type\":\"Successful\",\"message\":\"Tweet %s successfully deleted.\"}\n", tweet_id);
	}
	return respond;
}
char * TranslateReTweet(char * command) {
	char token[33], tweet_id[10];
	int command_counter = 8, token_counter = 0, tweet_id_counter = 0;
	while (strncmp(&command[command_counter], ", ", 2) != 0) {
		token[token_counter] = command[command_counter];
		token_counter++;
		command_counter++;
	}
	token[token_counter] = '\0';
	command_counter += 2;
	while (command[command_counter] != '\n') {
		tweet_id[tweet_id_counter] = command[command_counter];
		tweet_id_counter++;
		command_counter++;
	}
	tweet_id[tweet_id_counter] = '\0';
	char * username = NULL;
	int status = ReTweet(tweet_id, token, &username);
	char * respond = NULL;
	if (status == 0) {
		printf("Tweet with this id is not available.\n");
		respond = "{\"type\":\"Error\",\"message\":\"Tweet with this id is not available.\"}\n";
	}
	else if (status == 1) {
		respond = (char *)malloc(sizeof(char) * 75);
		printf("User %s successfully retweeted tweet %s.\n", username, tweet_id);
		sprintf(respond, "{\"type\":\"Successful\",\"message\":\"Tweet %s successfully retweeted.\"}\n", tweet_id);
	}
	return respond;
}
char * TranslateAdvanceSearch(char * command) {
	char token[33], username_to_search[50];
	int command_counter = 15, token_counter = 0, username_to_search_counter = 0;
	while (strncmp(&command[command_counter], ", ", 2) != 0) {
		token[token_counter] = command[command_counter];
		token_counter++;
		command_counter++;
	}
	token[32] = '\0';
	command_counter += 2;
	while (command[command_counter] != '\n') {
		username_to_search[username_to_search_counter] = command[command_counter];
		username_to_search_counter++;
		command_counter++;
	}
	username_to_search[username_to_search_counter] = '\0';
	char * username = NULL;
	char * status = AdvanceSearch(username_to_search, token, &username);
	char * respond = NULL;
	if (status == NULL) {
		printf("This username is not valid.\n");
		respond = "{\"type\":\"Error\",\"message\":\"This username is not valid.\"}\n";
	}
	else {
		respond = (char *)malloc(sizeof(char) * (strlen(status) + 35));
		printf("User %s successfully searched users by %s.\n", username, username_to_search);
		sprintf(respond, "{\"type\":\"Search\",\"message\":%s}\n", status);
	}
	return respond;
}
char * TranslateSearchTweet(char * command) {
	char token[33], word_to_search[50];
	int command_counter = 13, token_counter = 0, word_to_search_counter = 0;
	while (strncmp(&command[command_counter], ", ", 2) != 0) {
		token[token_counter] = command[command_counter];
		token_counter++;
		command_counter++;
	}
	token[32] = '\0';
	command_counter += 2;
	while (command[command_counter] != '\n') {
		word_to_search[word_to_search_counter] = command[command_counter];
		word_to_search_counter++;
		command_counter++;
	}
	word_to_search[word_to_search_counter] = '\0';
	char * username = NULL;
	char * result = SearchTweet(word_to_search, token, &username);
	char * respond = NULL;
	if (result == NULL) {
		printf("No tweet is found.\n");
		respond = "{\"type\":\"Error\",\"message\":\"No tweet is found.\"}\n";
	}
	else {
		printf("Tweets with #%s were sent.\n", word_to_search);
		respond = (char *)malloc(sizeof(char) * (strlen(result) + 35));
		sprintf(respond, "{\"type\":\"Search\",\"message\":%s}\n", result);
	}
	return respond;
}

//Process
int SignUp(char * username, char * password) {
	char file_name[100];
	sprintf(file_name, "%s.user.json", username);
	if (FileExists(file_name) == 1)
		return 0;
	MakeUserFile(username, password, "", "", "", "");
	//all users file
	char * all_users_file_name = "allusers.json";
	FILE * all_users_file_write = fopen(all_users_file_name, "a");
	if (all_users_file_write == NULL) 
		all_users_file_write = fopen(all_users_file_name, "w+");
	fprintf(all_users_file_write, "\"%s\",", username);
	fclose(all_users_file_write);
	return 1;
}
int LogIn(char * username, char * password) {
	char file_name[100];
	sprintf(file_name, "%s.user.json", username);
	if (FileExists(file_name) == 0)
		return 0;
	else {
		if (strcmp(password, ParsePassword(file_name)) == 0) 
			return 2;
		else
			return 1;
	}
}
int SendTweet(char * tweet, char * token, char ** username) {
	*username = FindUserViaToken(token);
	int tweet_id = 0;
	char file_name[50];
	do {
		tweet_id++;
		sprintf(file_name, "%d.tweet.json", tweet_id);
	} while (FileExists(file_name) == 1);
	MakeTweetFile(tweet_id, *username, tweet, "", 0);
	//User File
	char user_file_name[100];
	sprintf(user_file_name, "%s.user.json", *username);
	char * tweet_ids = ParsePersonalTweets(user_file_name);
	tweet_ids = (char *)realloc(tweet_ids,sizeof(char) * (strlen(tweet_ids) + 50));
	if (strlen(tweet_ids) < 1)
		sprintf(tweet_ids, "%d", tweet_id);
	else
		sprintf(tweet_ids, "%s,%d", tweet_ids, tweet_id);
	MakeUserFile(*username, ParsePassword(user_file_name), ParseBio(user_file_name), ParseFollowings(user_file_name), ParseFollowers(user_file_name), tweet_ids);
	return 1;
}
char * Refresh(char * token, char ** username) {
	char * result = (char *)calloc(sizeof(char), 1000000);
	int result_size = 1;
	*username = FindUserViaToken(token);
	//Finding Followings
	char file_name[100];
	sprintf(file_name, "%s.user.json", *username);
	char * followings = ParseFollowings(file_name);
	//Get Tweets
	int tweet_id = 1;
	sprintf(file_name, "%d.tweet.json", tweet_id);
	while (FileExists(file_name) == 1) {
		tweet_id++;
		sprintf(file_name, "%d.tweet.json", tweet_id);
	}
	tweet_id--;
	sprintf(file_name, "%d.tweet.json", tweet_id);
	while (tweet_id > 0) {
		if (strlen(ParseAuthor(file_name)) > 0) {
			if (IsAuthorFollowed(ParseAuthor(file_name), followings) == 1) {
				if (HasTweetReported(ParseTweetId(file_name), *username) == 0) {
					FILE * tweet_file = fopen(file_name, "r");
					int tweet_file_size = FindFileSize(tweet_file);
					char * tweet_file_data = (char *)malloc(sizeof(char) * (tweet_file_size + 1));
					fread(tweet_file_data, sizeof(char), tweet_file_size, tweet_file);
					tweet_file_data[tweet_file_size - 1] = '\0';
					fclose(tweet_file);
					if (strlen(result) <= 1)
						sprintf(result, "%s%s", result, tweet_file_data);
					else
						sprintf(result, "%s,%s", result, tweet_file_data);
				}
			}
		}
		tweet_id--;
		sprintf(file_name, "%d.tweet.json", tweet_id);
	}
	return result;
}
char * Search(char * user_to_search, char * token, char ** username) {
	*username = FindUserViaToken(token);
	char * result = (char *)malloc(sizeof(char) * 1000000);
	char file_name[100];
	sprintf(file_name, "%s.user.json", user_to_search);
	if (FileExists(file_name) == 0)
		return NULL;
	sprintf(result, "{\"username\":\"%s\",\"bio\":\"%s\",\"numberofFollowers\":%d,\"numberofFollowings\":%d,\"followStatus\":\"%s\",\"allTweets\":[",
		user_to_search, ParseBio(file_name), NumberOfFollowers(file_name), NumberOfFollowings(file_name), FollowStatus(*username, file_name));
	//Tweets
	char * tweet_ids = ParsePersonalTweets(file_name), tweet_id[10];
	int tweet_ids_index = 0, tweet_id_index = 0;
	while (tweet_ids[tweet_ids_index] != '\0') {
		if (tweet_ids[tweet_ids_index] == ',') {
			tweet_ids_index++;
			tweet_id[tweet_id_index] = '\0';
			tweet_id_index = 0;

			char tweet_file_name[50];
			sprintf(tweet_file_name, "%s.tweet.json", tweet_id);
			FILE * tweet_file = fopen(tweet_file_name, "r");
			int tweet_file_size = FindFileSize(tweet_file);
			char * tweet_file_data = (char *)malloc(sizeof(char) * (tweet_file_size + 1));
			fread(tweet_file_data, sizeof(char), tweet_file_size, tweet_file);
			tweet_file_data[tweet_file_size - 1] = '\0';
			sprintf(result, "%s%s,", result, tweet_file_data);
		}
		tweet_id[tweet_id_index] = tweet_ids[tweet_ids_index];
		tweet_ids_index++;
		tweet_id_index++;
	}
	tweet_id[tweet_id_index] = '\0';
	char tweet_file_name[50];
	sprintf(tweet_file_name, "%s.tweet.json", tweet_id);
	if (FileExists(tweet_file_name) == 1) {
		FILE * tweet_file = fopen(tweet_file_name, "r");
		int tweet_file_size = FindFileSize(tweet_file);
		char * tweet_file_data = (char *)malloc(sizeof(char) * (tweet_file_size + 1));
		fread(tweet_file_data, sizeof(char), tweet_file_size, tweet_file);
		tweet_file_data[tweet_file_size - 1] = '\0';
		sprintf(result, "%s%s]}", result, tweet_file_data);
	}
	else
		sprintf(result, "%s]", result);
	return result;
}
int Like(char * tweet_id, char * token, char ** username) {
	*username = FindUserViaToken(token);
	char file_name[50];
	sprintf(file_name, "%s.tweet.json", tweet_id);
	if (FileExists(file_name) == 0)
		return 0;
	if (strlen(ParseAuthor(file_name)) < 1)
		return 0;
	int likes = ParseLikes(file_name);
	likes++;
	MakeTweetFile(atoi(tweet_id), ParseAuthor(file_name), ParseContent(file_name), ParseComments(file_name), likes);
	return 1;
}
int Comment(char * tweet_id, char * comment, char * token, char ** username) {
	*username = FindUserViaToken(token);
	char file_name[50];
	sprintf(file_name, "%s.tweet.json", tweet_id);
	if (FileExists(file_name) == 0)
		return 0;
	if (strlen(ParseAuthor(file_name)) < 1)
		return 0;
	char * comments = ParseComments(file_name);
	int new_comment_size = strlen(*username) + strlen(comment) + 7;
	char * new_comment = (char *)malloc(sizeof(char) * (new_comment_size));
	sprintf(new_comment, "\"%s\":\"%s\"", *username, comment);
	comments = (char *)realloc(comments, sizeof(char) * (strlen(comments) + new_comment_size));
	if (strlen(comments) < 4)
		sprintf(comments, "%s%s", comments, new_comment);
	else
		sprintf(comments, "%s,%s", comments, new_comment);
	MakeTweetFile(atoi(tweet_id), ParseAuthor(file_name), ParseContent(file_name), comments, ParseLikes(file_name));
	return 1;
}
int Follow(char * user_to_follow, char * token, char ** username) {
	//Following Handling
	*username = FindUserViaToken(token);
	char file_name[100];
	sprintf(file_name, "%s.user.json", *username);
	char * final_followings = ParseFollowings(file_name);
	int current_following_size = strlen(final_followings);
	int final_followings_size = strlen(user_to_follow) + strlen(final_followings) + 1;
	final_followings = (char *)realloc(final_followings, final_followings_size);
	final_followings[current_following_size - 1] = '\"';
	final_followings[current_following_size] = '\0';
	if (strlen(final_followings) < 1)
		sprintf(final_followings, "%s\"%s\"", final_followings, user_to_follow);
	else
		sprintf(final_followings, "%s,\"%s\"", final_followings, user_to_follow);
	MakeUserFile(*username, ParsePassword(file_name), ParseBio(file_name), final_followings, ParseFollowers(file_name), ParsePersonalTweets(file_name));
	//Follower Handling
	char des_file_name[100];
	sprintf(des_file_name, "%s.user.json", user_to_follow);
	char * final_followers = ParseFollowers(des_file_name);
	int current_follower_size = strlen(final_followers);
	int final_followes_size = strlen(*username) + strlen(final_followers) + 3;
	final_followers = (char *)realloc(final_followers, final_followes_size);
	final_followers[current_follower_size] = '\"';
	final_followers[current_follower_size] = '\0';
	if (strlen(final_followers) < 1)
		sprintf(final_followers, "%s\"%s\"", final_followers, *username);
	else
		sprintf(final_followers, "%s,\"%s\"", final_followers, *username);
	MakeUserFile(user_to_follow, ParsePassword(des_file_name), ParseBio(des_file_name), ParseFollowings(des_file_name), final_followers, ParsePersonalTweets(des_file_name));
	return 1;
}
int UnFollow(char * user_to_unfollow, char * token, char ** username) {
	//Following Handling
	*username = FindUserViaToken(token);
	char file_name[100];
	sprintf(file_name, "%s.user.json", *username);
	char * followings = ParseFollowings(file_name);
	char * final_followings = (char *)malloc(sizeof(char) * strlen(followings));
	int followings_counter = 0, final_followings_counter = 0;
	while (followings[followings_counter] != '\0') {
		final_followings[final_followings_counter] = followings[followings_counter];
		final_followings_counter++;
		followings_counter++;
		if (strncmp(&followings[followings_counter], user_to_unfollow, strlen(user_to_unfollow)) == 0) {
			followings_counter += strlen(user_to_unfollow) + 1;
			if (followings[followings_counter] == ',')
				followings_counter++;
			final_followings_counter--;
		}
	}
	final_followings[final_followings_counter] = '\0';
	MakeUserFile(*username, ParsePassword(file_name), ParseBio(file_name), final_followings, ParseFollowers(file_name), ParsePersonalTweets(file_name));
	//Follower Handling
	char des_file_name[100];
	sprintf(des_file_name, "%s.user.json", user_to_unfollow);
	char * followers = ParseFollowers(des_file_name);
	char * final_followers = (char *)malloc(sizeof(char) * strlen(followers));
	int followers_counter = 0, final_followers_counter = 0;
	while (followers[followers_counter] != '\0') {
		final_followers[final_followers_counter] = followers[followers_counter];
		final_followers_counter++;
		followers_counter++;
		if (strncmp(&followers[followers_counter], *username, strlen(*username)) == 0) {
			followers_counter += strlen(*username) + 1;
			if (followers[followers_counter] == ',')
				followers_counter++;
			final_followers_counter--;
		}
	}
	final_followers[final_followers_counter] = '\0';
	MakeUserFile(user_to_unfollow, ParsePassword(des_file_name), ParseBio(des_file_name), ParseFollowings(des_file_name), final_followers, ParsePersonalTweets(des_file_name));
	return 1;
}
int SetBio(char * bio, char * token, char ** username) {
	*username = FindUserViaToken(token);
	char file_name[100];
	sprintf(file_name, "%s.user.json", *username);
	MakeUserFile(*username, ParsePassword(file_name), bio, ParseFollowings(file_name), ParseFollowers(file_name), ParsePersonalTweets(file_name));
	return 1;
}
char * Profile(char * token, char ** username) {
	*username = FindUserViaToken(token);
	char * result = (char *)malloc(sizeof(char) * 1000000);
	char file_name[100];
	sprintf(file_name, "%s.user.json", *username);
	sprintf(result, "{\"username\":\"%s\",\"bio\":\"%s\",\"numberofFollowers\":%d,\"numberofFollowings\":%d,\"followStatus\":\"Yourself\",\"allTweets\":[",
		*username, ParseBio(file_name), NumberOfFollowers(file_name), NumberOfFollowings(file_name));
	//Tweets
	char * tweet_ids = ParsePersonalTweets(file_name), tweet_id[10];
	int tweet_ids_index = 0, tweet_id_index = 0;
	while (tweet_ids[tweet_ids_index] != '\0') {
		if (tweet_ids[tweet_ids_index] == ',') {
			tweet_ids_index++;
			tweet_id[tweet_id_index] = '\0';
			tweet_id_index = 0;

			char tweet_file_name[50];
			sprintf(tweet_file_name, "%s.tweet.json", tweet_id);
			FILE * tweet_file = fopen(tweet_file_name, "r");
			int tweet_file_size = FindFileSize(tweet_file);
			char * tweet_file_data = (char *)malloc(sizeof(char) * (tweet_file_size + 1));
			fread(tweet_file_data, sizeof(char), tweet_file_size, tweet_file);
			tweet_file_data[tweet_file_size - 1] = '\0';
			sprintf(result, "%s%s,", result, tweet_file_data);
		}
		tweet_id[tweet_id_index] = tweet_ids[tweet_ids_index];
		tweet_ids_index++;
		tweet_id_index++;
	}
	tweet_id[tweet_id_index] = '\0';
	char tweet_file_name[50];
	sprintf(tweet_file_name, "%s.tweet.json", tweet_id);
	if (FileExists(tweet_file_name) == 1) {
		FILE * tweet_file = fopen(tweet_file_name, "r");
		int tweet_file_size = FindFileSize(tweet_file);
		char * tweet_file_data = (char *)malloc(sizeof(char) * (tweet_file_size + 1));
		fread(tweet_file_data, sizeof(char), tweet_file_size, tweet_file);
		tweet_file_data[tweet_file_size - 1] = '\0';
		sprintf(result, "%s%s]}", result, tweet_file_data);
	}
	else
		sprintf(result, "%s]", result);
	return result;
}
int ChangePassword(char * current_password, char * new_password, char * token, char ** username) {
	*username = FindUserViaToken(token);
	char file_name[100];
	sprintf(file_name, "%s.user.json", *username);
	if (strcmp(ParsePassword(file_name), current_password) != 0)
		return 0;
	if (strcmp(current_password, new_password) == 0)
		return 0;
	MakeUserFile(*username, new_password, ParseBio(file_name), ParseFollowings(file_name), ParseFollowers(file_name), ParsePersonalTweets(file_name));
	return 1;
}
int DeleteTweet(char * tweet_id, char * token, char ** username) {
	*username = FindUserViaToken(token);
	char file_name[50];
	sprintf(file_name, "%s.tweet.json", tweet_id);
	if (FileExists(file_name) == 0)
		return 0;
	if (strlen(ParseAuthor(file_name)) < 1)
		return 0;
	if (strcmp(ParseAuthor(file_name), *username) != 0)
		return 1;
	MakeTweetFile(atoi(tweet_id), "", "", "", 0);
	//Tweet id
	char user_file_name[50];
	sprintf(user_file_name, "%s.user.json", *username);
	char * tweet_ids = ParsePersonalTweets(user_file_name);
	char * final_tweet_ids = (char *)malloc(sizeof(char) * strlen(tweet_ids));
	int tweet_ids_index = 0, final_tweet_ids_index = 0;
	while (tweet_ids[tweet_ids_index] != '\0' && tweet_ids[tweet_ids_index - 1] != '\0') {
		if (strncmp(&tweet_ids[tweet_ids_index], tweet_id, strlen(tweet_id)) == 0) {
			int flag = 0;
			if (tweet_ids[tweet_ids_index - 1] == ',') {
				flag = 1;
				final_tweet_ids_index--;
			}
			tweet_ids_index += strlen(tweet_id);
			if (tweet_ids[tweet_ids_index] == ',' && flag == 0)
				tweet_ids_index++;
		}
		final_tweet_ids[final_tweet_ids_index] = tweet_ids[tweet_ids_index];
		final_tweet_ids_index++;
		tweet_ids_index++;
	}
	final_tweet_ids[final_tweet_ids_index] = '\0';
	MakeUserFile(*username, ParsePassword(user_file_name), ParseBio(user_file_name), ParseFollowings(user_file_name), ParseFollowers(user_file_name), final_tweet_ids);
	return 2;
}
int ReTweet(char * tweet_id, char * token, char ** username) {
	*username = FindUserViaToken(token);
	char file_name[50];
	sprintf(file_name, "%s.tweet.json", tweet_id);
	if (FileExists(file_name) == 0)
		return 0;
	char * content = (char *)malloc(sizeof(char) * 400);
	sprintf(content, "RETWEETED FROM (%s)\n%s", ParseAuthor(file_name), ParseContent(file_name));
	return SendTweet(content, token, username);
}
int LogOut(char * token, char ** username) {
	*username = FindUserViaToken(token);
	int index = SearchUsersViaUserName(*username);
	return 1;
}
char * AdvanceSearch(char * user_to_search, char * token, char ** username) {
	*username = FindUserViaToken(token);
	char * result = (char *)malloc(sizeof(char) * 1000000);
	char * file_name = "allusers.json";
	FILE * users_file = fopen(file_name, "r");
	int file_size = FindFileSize(users_file);
	char * file_data = (char *)malloc(sizeof(char) * (file_size + 50));
	fread(file_data, sizeof(char), file_size, users_file);
	file_data[file_size - 1] = '\0';
	fclose(users_file);
	int file_data_index = 0, result_index = 0;
	char wanting_word[50];
	int flag = 0;
	sprintf(wanting_word, "\"%s", user_to_search);
	while (file_data[file_data_index] != '\0') {
		if (strncmp(&file_data[file_data_index], wanting_word, strlen(wanting_word)) == 0) {
			flag = 1;//&& strncmp(&file_data[file_data_index + 1], *username, strlen(*username)) != 0) {
			while (file_data[file_data_index] != ',') {
				result[result_index] = file_data[file_data_index];
				result_index++;
				file_data_index++;
			}
			result[result_index] = file_data[file_data_index];
			result_index++;
		}
		file_data_index++;
	}
	if (flag == 0)
		return NULL;
	result[result_index - 1] = '\0';
	return result;
}
char * SearchTweet(char * word, char * token, char ** username) {
	*username = FindUserViaToken(token);
	char * result = (char *)malloc(sizeof(char) * 1000000);
	result[0] = '\0';
	char file_name[100];
	int tweet_id = 1, flag = 0;
	sprintf(file_name, "%d.tweet.json", tweet_id);
	while (FileExists(file_name) == 1) {
		char * content = ParseContent(file_name);
		if (SearchWord(word, content) == 1) {
			flag ++;
			FILE * tweet_file = fopen(file_name, "r");
			int tweet_file_size = FindFileSize(tweet_file);
			char * tweet_file_data = (char *)malloc(sizeof(char) * (tweet_file_size + 1));
			fread(tweet_file_data, sizeof(char), tweet_file_size, tweet_file);
			tweet_file_data[tweet_file_size - 1] = '\0';
			if (flag > 1)
				sprintf(result, "%s,%s", result, tweet_file_data);
			else
				sprintf(result, "%s%s", result, tweet_file_data);
		}
		tweet_id++;
		sprintf(file_name, "%d.tweet.json", tweet_id);
	}
	if (flag == 0)
		return NULL;
	return result;
}


void MakeUserFile(char * username, char * password, char * bio, char * followings, char * followers, char * personal_tweet) {
	char file_name[100];
	sprintf(file_name, "%s.user.json", username);
	FILE * new_user_file = fopen(file_name, "w+");
	char file_data[1000000];
	sprintf(file_data, "{\"username\":\"%s\",\"password\":\"%s\",\"bio\":\"%s\",\"followers\":[%s],\"followings\":[%s],\"personalTweets\":[%s]}"
		, username, password, bio, followers, followings, personal_tweet);
	fwrite(file_data, sizeof(char), strlen(file_data), new_user_file);
	fclose(new_user_file);
}
void MakeUserStruct(char * username) {
	if (SearchUsersViaUserName(username) == -1) {
		users = (User *)realloc(users, sizeof(User) * (users_counter + 1));
		users[users_counter].username = (char *)malloc(sizeof(char) * (strlen(username) + 1));
		strcpy(users[users_counter].username, username);
		users[users_counter].tweets_num_viewed = 0;
		users[users_counter].tweet_ids_viewed = NULL;
		users_counter++;
	}
}
char * MakeToken(char * username) {
	int index = SearchUsersViaUserName(username);
	srand((int)time(0));
	users[index].token = (char *)malloc(sizeof(char) * 33);
	do {
		for (int i = 0; i < 33; i++) {
			users[index].token[i] = RandomCharGenerator();
		}
		users[index].token[32] = '\0';
	} while (IsTokenUnic(users[index].token) == 0);
	return users[index].token;
}
int SearchUsersViaUserName(char * username) {
	for (int i = 0; i < users_counter; i++) {
		if (strcmp(username, users[i].username) == 0)
			return i;
	}
	return -1;
}
int IsTokenUnic(char * token) {
	int token_counter = 0;
	for (int i = 0; i < users_counter; i++) {
		if (strcmp(token, users[i].token) == 0)
			token_counter++;
	}
	if (token_counter > 1)
		return 0;
	return 1;
}
char * FindUserViaToken(char * token) {
	for (int i = 0; i < users_counter; i++) {
		if (strcmp(users[i].token, token) == 0)
			return users[i].username;
	}
	return NULL;
}
void MakeTweetFile(int tweet_id, char * username, char * tweet, char * comment, int likes) {
	char file_name[50];
	sprintf(file_name, "%d.tweet.json", tweet_id);
	FILE * new_tweet_file = fopen(file_name, "w+");
	char file_data[1000000];
	sprintf(file_data, "{\"id\":%d,\"author\":\"%s\",\"content\":\"%s\",\"comments\":{%s},\"likes\":%d}", tweet_id, username, tweet, comment, likes);
	fwrite(file_data, sizeof(char), strlen(file_data), new_tweet_file);
	fclose(new_tweet_file);
}
int IsAuthorFollowed(char * author, char * followings) {
	int following_counter = 0;
	while (followings[following_counter] != '\0') {
		if (strncmp(&followings[following_counter], author, strlen(author)) == 0)
			return 1;
		following_counter++;
	}
	return 0;
}
int HasTweetReported(int tweet_id, char * username) {
	int index = SearchUsersViaUserName(username);
	for (int i = 0; i < users[index].tweets_num_viewed; i++) {
		if (tweet_id == users[index].tweet_ids_viewed[i])
			return 1;
	}
	users[index].tweets_num_viewed++;
	users[index].tweet_ids_viewed = (int *)realloc(users[index].tweet_ids_viewed, sizeof(int) * users[index].tweets_num_viewed);
	users[index].tweet_ids_viewed[users[index].tweets_num_viewed - 1] = tweet_id;
	return 0;
}
int NumberOfFollowers(char * file_name) {
	char * followers = ParseFollowers(file_name);
	if (strlen(followers) < 2)
		return 0;
	int followers_counter = 1, followers_index = 0;
	while (followers[followers_index] != '\0') {
		if (followers[followers_index] == ',')
			followers_counter++;
		followers_index++;
	}
	return followers_counter;
}
int NumberOfFollowings(char * file_name) {
	char * followings = ParseFollowings(file_name);
	if (strlen(followings) < 2)
		return 0;
	int followings_counter = 1, followings_index = 0;
	while (followings[followings_index] != '\0') {
		if (followings[followings_index] == ',')
			followings_counter++;
		followings_index++;
	}
	return followings_counter;
}
char * FollowStatus(char * username, char * file_name) {
	char * followers = ParseFollowers(file_name);
	int followers_index = 0;
	char * followed = "Followed";
	while (followers[followers_index] != '\0') {
		if (strncmp(&followers[followers_index], username, strlen(username)) == 0)
			return followed;
		followers_index++;
	}
	char * not_followed = "NotFollowed";
	return not_followed;
}
int SearchWord(char * word, char * content) {
	char * word_to_search = (char *)malloc(sizeof(char) * (strlen(word) + 2));
	sprintf(word_to_search, "#%s", word);
	int content_index = 0;
	while (content[content_index] != '\0') {
		if (strncmp(&content[content_index], word_to_search, strlen(word_to_search)) == 0) {
			return 1;
		}
		content_index++;
	}
	return 0;
}

char RandomCharGenerator() {
	int a;
	do {
		a = (rand() % 90) + 33;
	} while (!((a > 47 && a < 58) || (a > 64 && a < 91) || (a > 95 && a < 123)));
	return (char)a;
}
int FileExists(const char * filename) {
	FILE *file;
	if (file = fopen(filename, "r")) {
		fclose(file);
		return 1;
	}
	return 0;
}
int FindFileSize(FILE * file) {
	int file_data_counter = 0;
	while (!feof(file)) {
		fgetc(file);
		file_data_counter++;
	}
	rewind(file);
	return file_data_counter;
}

//Parsing Users
char * ParseUsername(char * file_name) {
	FILE * user_file = fopen(file_name, "r");
	int user_file_size = FindFileSize(user_file);
	int user_file_index = 0, username_index = 0;
	char * username = (char *)malloc(sizeof(char) * 50);
	char * user_file_data = (char *)malloc(sizeof(char) * (user_file_size + 1));
	fread(user_file_data, sizeof(char), user_file_size, user_file);
	while (strncmp(&user_file_data[user_file_index], "\"username\":\"", 12) != 0)
		user_file_index++;
	user_file_index += 12;
	while (user_file_data[user_file_index] != '\"') {
		username[username_index] = user_file_data[user_file_index];
		username_index++;
		user_file_index++;
	}
	username[username_index] = '\0';
	fclose(user_file);
	return username;
}
char * ParsePassword(char * file_name) {
	FILE * user_file = fopen(file_name, "r");
	int user_file_size = FindFileSize(user_file);
	int user_file_index = 0, password_index = 0;
	char * password = (char *)malloc(sizeof(char) * 50);
	char * user_file_data = (char *)malloc(sizeof(char) * (user_file_size + 1));
	fread(user_file_data, sizeof(char), user_file_size, user_file);
	while (strncmp(&user_file_data[user_file_index], "\"password\":\"", 12) != 0)
		user_file_index++;
	user_file_index += 12;
	while (user_file_data[user_file_index] != '\"') {
		password[password_index] = user_file_data[user_file_index];
		password_index++;
		user_file_index++;
	}
	password[password_index] = '\0';
	fclose(user_file);
	return password;
}
char * ParseBio(char * file_name) {
	FILE * user_file = fopen(file_name, "r");
	int user_file_size = FindFileSize(user_file);
	int user_file_index = 0, bio_index = 0;
	char * bio = (char *)malloc(sizeof(char) * 200);
	char * user_file_data = (char *)malloc(sizeof(char) * (user_file_size + 1));
	fread(user_file_data, sizeof(char), user_file_size, user_file);
	while (strncmp(&user_file_data[user_file_index], "\"bio\":\"", 7) != 0)
		user_file_index++;
	user_file_index += 7;
	while (user_file_data[user_file_index] != '\"') {
		bio[bio_index] = user_file_data[user_file_index];
		bio_index++;
		user_file_index++;
	}
	bio[bio_index] = '\0';
	fclose(user_file);
	return bio;
}
char * ParseFollowings(char * file_name) {
	char * followings = (char *)malloc(sizeof(char) * 1000000);
	int followings_index = 0, user_file_index = 0;
	FILE * user_file = fopen(file_name, "r");
	int user_file_size = FindFileSize(user_file);
	char * user_file_data = (char *)malloc(sizeof(char) * (user_file_size + 1));
	fread(user_file_data, sizeof(char), user_file_size, user_file);
	while (strncmp(&user_file_data[user_file_index], "\"followings\":[", 14) != 0)
		user_file_index++;
	user_file_index += 14;
	while (user_file_data[user_file_index] != ']') {
		followings[followings_index] = user_file_data[user_file_index];
		followings_index++;
		user_file_index++;
	}
	followings[followings_index] = '\0';
	fclose(user_file);
	return followings;
}
char * ParseFollowers(char * file_name) {
	char * followers = (char *)malloc(sizeof(char) * 1000000);
	int followers_index = 0, user_file_index = 0;
	FILE * user_file = fopen(file_name, "r");
	int user_file_size = FindFileSize(user_file);
	char * user_file_data = (char *)malloc(sizeof(char) * (user_file_size + 1));
	fread(user_file_data, sizeof(char), user_file_size, user_file);
	while (strncmp(&user_file_data[user_file_index], "\"followers\":[", 13) != 0)
		user_file_index++;
	user_file_index += 13;
	while (user_file_data[user_file_index] != ']') {
		followers[followers_index] = user_file_data[user_file_index];
		followers_index++;
		user_file_index++;
	}
	followers[followers_index] = '\0';
	fclose(user_file);
	return followers;
}
char * ParsePersonalTweets(char * file_name) {
	char * personal_tweets = (char *)malloc(sizeof(char) * 1000);
	int personal_tweets_index = 0, file_data_index = 0;
	FILE * user_file = fopen(file_name, "r");
	int user_file_size = FindFileSize(user_file);
	char * user_file_data = (char *)malloc(sizeof(char) * (user_file_size + 1));
	fread(user_file_data, sizeof(char), user_file_size, user_file);
	while (strncmp(&user_file_data[file_data_index], "\"personalTweets\":[", 18) != 0)
		file_data_index++;
	file_data_index += 18;
	while (user_file_data[file_data_index] != ']') {
		personal_tweets[personal_tweets_index] = user_file_data[file_data_index];
		personal_tweets_index++;
		file_data_index++;
	}
	personal_tweets[personal_tweets_index] = '\0';
	fclose(user_file);
	return personal_tweets;
}
//Parsing Tweets
int ParseTweetId(char * file_name) {
	FILE * tweet_file = fopen(file_name, "r");
	int tweet_file_size = FindFileSize(tweet_file);
	int tweet_file_index = 0, tweet_id_index = 0;
	char * tweet_id = (char *)malloc(sizeof(char) * 10);
	char * tweet_file_data = (char *)malloc(sizeof(char) * (tweet_file_size + 1));
	fread(tweet_file_data, sizeof(char), tweet_file_size, tweet_file);
	while (strncmp(&tweet_file_data[tweet_file_index], "\"id\":", 5) != 0)
		tweet_file_index++;
	tweet_file_index += 5;
	while (tweet_file_data[tweet_file_index] != ',') {
		tweet_id[tweet_id_index] = tweet_file_data[tweet_file_index];
		tweet_id_index++;
		tweet_file_index++;
	}
	tweet_id[tweet_id_index] = '\0';
	int tweetid = atoi(tweet_id);
	return tweetid;
}
char * ParseAuthor(char * file_name) {
	FILE * tweet_file = fopen(file_name, "r");
	int tweet_file_size = FindFileSize(tweet_file);
	int tweet_file_index = 0, author_index = 0;
	char * author = (char *)malloc(sizeof(char) * 50);
	char * tweet_file_data = (char *)malloc(sizeof(char) * (tweet_file_size + 1));
	fread(tweet_file_data, sizeof(char), tweet_file_size, tweet_file);
	while (strncmp(&tweet_file_data[tweet_file_index], "\"author\":\"", 10) != 0)
		tweet_file_index++;
	tweet_file_index += 10;
	while (tweet_file_data[tweet_file_index] != '\"') {
		author[author_index] = tweet_file_data[tweet_file_index];
		author_index++;
		tweet_file_index++;
	}
	author[author_index] = '\0';
	fclose(tweet_file);
	return author;
}
char * ParseContent(char * file_name) {
	FILE * tweet_file = fopen(file_name, "r");
	int tweet_file_size = FindFileSize(tweet_file);
	int tweet_file_index = 0, content_index = 0;
	char * content = (char *)malloc(sizeof(char) * 300);
	char * tweet_file_data = (char *)malloc(sizeof(char) * (tweet_file_size + 1));
	fread(tweet_file_data, sizeof(char), tweet_file_size, tweet_file);
	while (strncmp(&tweet_file_data[tweet_file_index], "\"content\":\"", 11) != 0)
		tweet_file_index++;
	tweet_file_index += 11;
	while (tweet_file_data[tweet_file_index] != '\"') {
		content[content_index] = tweet_file_data[tweet_file_index];
		content_index++;
		tweet_file_index++;
	}
	content[content_index] = '\0';
	fclose(tweet_file);
	return content;
}
char * ParseComments(char * file_name) {
	char * comments = (char *)malloc(sizeof(char) * 1000000);
	int comment_index = 0, tweet_file_data_index = 0;
	FILE * tweet_file = fopen(file_name, "r");
	int tweet_file_size = FindFileSize(tweet_file);
	char * tweet_file_data = (char *)malloc(sizeof(char) * (tweet_file_size + 1));
	fread(tweet_file_data, sizeof(char), tweet_file_size, tweet_file);
	while (strncmp(&tweet_file_data[tweet_file_data_index], "\"comments\":{", 12) != 0) 
		tweet_file_data_index++;
	tweet_file_data_index += 12;
	while (tweet_file_data[tweet_file_data_index] != '}') {
		comments[comment_index] = tweet_file_data[tweet_file_data_index];
		comment_index++;
		tweet_file_data_index++;
	}
	comments[comment_index] = '\0';
	fclose(tweet_file);
	return comments;
}
int ParseLikes(char * file_name) {
	FILE * tweet_file = fopen(file_name, "r");
	int tweet_file_size = FindFileSize(tweet_file);
	int tweet_file_index = 0, like_index = 0;
	char * like_str = (char *)malloc(sizeof(char) * 10);
	char * tweet_file_data = (char *)malloc(sizeof(char) * (tweet_file_size + 1));
	fread(tweet_file_data, sizeof(char), tweet_file_size, tweet_file);
	while (strncmp(&tweet_file_data[tweet_file_index], "\"likes\":", 8) != 0)
		tweet_file_index++;
	tweet_file_index += 8;
	while (tweet_file_data[tweet_file_index] != '}') {
		like_str[like_index] = tweet_file_data[tweet_file_index];
		like_index++;
		tweet_file_index++;
	}
	like_str[like_index] = '\0';
	int like = atoi(like_str);
	return like;
}
