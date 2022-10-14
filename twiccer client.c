#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define RED     "\x1b[1;31m"
#define GREEN   "\x1b[1;32m"
#define YELLOW  "\x1b[1;33m"
#define BLUE    "\x1b[1;34m"
#define MAGENTA "\x1b[1;35m"
#define CYAN    "\x1b[1;36m"
#define RESET   "\x1b[0m"
#pragma comment (lib,"WS2_32.lib")
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#define PORT 12345

typedef struct TWEETCOLLECTOR TWEETCOLLECTOR;
typedef struct PROFILECOLLECTOR PROFILECOLLECTOR;
struct TWEETCOLLECTOR {
	char * id;
	char * username;
	char * content;
	char * like;
	int comment;
	char * comments;
};
struct PROFILECOLLECTOR {
	int error;
	char * username;
	char * bio;
	char * followers;
	char * followings;
	char * followstatus;
	TWEETCOLLECTOR * tweets;
};
struct sockaddr_in server;

//FRONT END
void Header();
void RegisterMenu();
void SignUp();
void LogIn();
void MainMenu();
void TimeLine();
void Search();
void ReTweet();
void AdvanceSearch();
void Follow(char * username);
void UnFollow(char * username);
void TweetProfile();
void PersonalArea();
void SendTweet();
void Refresh();
void DeleteTweet();
void LikeOrComment();
void Like();
void Comment();
void SetBio();
void ChangePassword();
void LogOutToServer();
void ShowComments();
void TweetSearch();

//TRNASLATOR
int SignUpToServer(char username[], char password[]);
int LogInToServer(char username[], char password[]);
TWEETCOLLECTOR * RefreshToServer();
void SendTweetToServer(char tweet[]);
int LikeToServer(int tweetnum);
int CommentToServer(int tweet_num, char comment[]);
PROFILECOLLECTOR SearchToServer(char username[], int * follow_status);
void FollowToServer(char username[]);
void UnFollowToServer(char username[]);
PROFILECOLLECTOR ProfileToServer();
int ChangePasswordToServer(char current_password[], char new_password[]);
int SetBioToServer(char bio[]);
char ** AdvanceSearchToServer(char * username);
TWEETCOLLECTOR * SearchTweetToServer();

//BACK END
char * ServerRespond(char * message_to_server);
int Initialize();

//STRING FUNCTIONS
char * CopyStr(char * input, char mode, int * counter);
int CompareStr(char str1[], char str2[]);
void PrintStr(char str[]);
char * StringGetter();
int StrToInt(char * str);
void CopyStrToStr(char * str1, char * str2);

//GLOBAL VARS
int n_index = 0;
char token[33];
char comments[2000][250];

int main() {
	for (int i = 0; i < 1000; i++) {
		comments[i][0] = '-';
		comments[i][1] = '1';
	}
	RegisterMenu();
	return 0;
}

//FRONT END
void Header() {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	int columns;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	for (int i = 0; i < (columns - 18) / 2; i++)
		printf(" ");
	printf(CYAN"Welcome to twiccer\n");
	for (int i = 0; i < (columns - 26) / 2; i++)
		printf(" ");
	printf("Created by: Mehdi Lotfian\n");
	for (int i = 0; i < columns; i++)
		printf("-");
	printf(RESET"\n");
}
void RegisterMenu() {
	int option = 0;
	while (option != 3 && option != 2 && option != 1) {
		system("cls");
		Header();
		printf("Register Menu:\n1.Sign Up\n2.Log In\n3.Exit\n");
		scanf("%d", &option);
		if (option == 1)
			SignUp();
		else if (option == 2)
			LogIn();
	}
	return;
}
void SignUp() {
	char username[20];
	char password[20];
	system("cls");
	Header();
	printf("Username:\n");
	scanf("%s", username);
	printf("Password:\n");
	scanf("%s", password);
	int response = SignUpToServer(username, password);
	if (response == 1) {
		int option = 3;
		while (option != 2 && option != 1) {
			system("cls");
			Header();
			printf(GREEN"You signed up successfully\n\n"RESET);
			printf("1.Log In\n2.Back\n");
			scanf("%d", &option);
			if (option == 1)
				LogIn();
			else if (option == 2)
				RegisterMenu();
		}
	}
	else if (response == 2) {
		int option = 3;
		while (option != 2 && option != 1) {
			system("cls");
			Header();
			printf(RED"This username is already taken\n\n"RESET);
			printf("1.Try Again\n2.Back\n");
			scanf("%d", &option);
			if (option == 1)
				SignUp();
			else if (option == 2)
				RegisterMenu();
		}
	}
	return;
}
void LogIn() {
	int i = 0;
	char username[20];
	char password[20];
	system("cls");
	Header();
	printf("Username:\n");
	scanf("%s", username);
	printf("Password:\n");
	scanf("%s", password);
	int response = LogInToServer(username, password);
	if (response == 1) {
		MainMenu();
	}
	else if (response == 2) {
		int option = 4;
		while (option != 3 && option != 2 && option != 1) {
			system("cls");
			Header();
			printf(RED"There is no account with this username\n\n"RESET);
			printf("1.Try Again\n2.Sign Up\n3.Back\n");
			scanf("%d", &option);
			if (option == 1)
				LogIn();
			else if (option == 2)
				SignUp();
			else if (option == 3)
				RegisterMenu();
		}
	}
	else if (response == 3) {
		int option = 4;
		while (option != 3 && option != 2 && option != 1) {
			system("cls");
			Header();
			printf(RED"The password is incorrect\n\n"RESET);
			printf("1.Try Again\n2.Sign Up\n3.Back\n");
			scanf("%d", &option);
			if (option == 1)
				LogIn();
			else if (option == 2)
				SignUp();
			else if (option == 3)
				RegisterMenu();
		}
	}
	return;
}
void MainMenu() {
	int option = 0;
	while (option != 6 && option != 5 && option != 4 && option != 3 && option != 2 && option != 1) {
		system("cls");
		Header();
		printf("1.Time Line\n2.Search User\n3.Search Tweet\n4.Tweet Profile\n5.Personal Area\n6.Log Out\n");
		scanf("%d", &option);
		if (option == 1)
			TimeLine();
		else if (option == 2)
			AdvanceSearch();
		else if (option == 3)
			TweetSearch();
		else if (option == 4)
			TweetProfile();
		else if (option == 5)
			PersonalArea();
		else if (option == 6) {
			LogOutToServer();
			RegisterMenu();
		}
	}
	return;
}
void TimeLine() {
	int option = 0;
	while (option != 5 && option != 4 && option != 3 && option != 2 && option != 1) {
		system("cls");
		Header();
		printf("1.Send Tweet\n2.Refresh\n3.Delete Tweet\n4.Retweet\n5.Back\n");
		scanf("%d", &option);
		if (option == 1)
			SendTweet();
		else if (option == 2)
			Refresh();
		else if (option == 3)
			DeleteTweet();
		else if (option == 4)
			ReTweet();
		else if (option == 5)
			MainMenu();
	}
	return;
}
void SendTweet() {
	system("cls");
	Header();
	char tweet[2000];
	printf("Enter the Tweet:\n");
	scanf("%*c%[^\n]s", tweet);
	SendTweetToServer(tweet);
	TimeLine();
	return;
}
void Refresh() {
	TWEETCOLLECTOR * message_from_server = RefreshToServer();
	int option = 0, i = 0;
	if (CompareStr(message_from_server[0].id, "Error") == 1) {
		int option = 0;
		while (option != 3 && option != 1 && option != 2) {
			system("cls");
			Header();
			printf(RED"There is no new tweet available\n\n"RESET);
			printf("1.Like or Comment a Tweet\n2.Refresh Again\n3.Back\n");
			scanf("%d", &option);
			if (option == 1)
				LikeOrComment();
			else if (option == 2)
				Refresh();
			else if (option == 3)
				TimeLine();
		}
	}
	else {
		while (option != 3 && option != 2 && option != 1) {
			system("cls");
			Header();
			while (message_from_server[i].username != NULL) {
				PrintStr(message_from_server[i].username);
				printf("\t");
				PrintStr(message_from_server[i].id);
				printf("\n");
				PrintStr(message_from_server[i].content);
				printf("\nlikes: ");
				PrintStr(message_from_server[i].like);
				printf("  comments: %d\n\n", message_from_server[i].comment);
				i++;
			}
			free(message_from_server);
			printf("1.Like or Comment a Tweet\n2.Refresh Again\n3.Back\n");
			scanf("%d", &option);
			if (option == 1)
				LikeOrComment();
			else if (option == 2)
				Refresh();
			else if (option == 3)
				TimeLine();
		}
	}
	return;
}
void DeleteTweet() {
	system("cls");
	Header();
	int tweet_num;
	printf("Enter the tweet Number: ");
	scanf("%d", &tweet_num);
	int response = DeleteTweetToServer(tweet_num);
	if (response == 1) {
		int option = 0;
		while (option != 2 && option != 1) {
			system("cls");
			Header();
			printf(GREEN"You successfully deleted tweet %d\n\n"RESET, tweet_num);
			printf("1.Delete another tweet\n2.Back\n");
			scanf("%d", &option);
			if (option == 1)
				DeleteTweet();
			else if (option == 2)
				TimeLine();
		}
	}
	else if (response == 2) {
		int option = 0;
		while (option != 2 && option != 1) {
			system("cls");
			Header();
			printf(RED"There is no tweet with this number\n\n"RESET);
			printf("1.Try Again\n2.Back\n");
			scanf("%d", &option);
			if (option == 1)
				DeleteTweet();
			else if (option == 2)
				TimeLine();
		}
	}
	else if (response == 3) {
		int option = 0;
		while (option != 2 && option != 1) {
			system("cls");
			Header();
			printf(RED"Tweet %d doesn't blong to you\n\n"RESET, tweet_num);
			printf("1.Try Again\n2.Back\n");
			scanf("%d", &option);
			if (option == 1)
				DeleteTweet();
			else if (option == 2)
				TimeLine();
		}
	}
	return;
}
void ReTweet() {
	system("cls");
	Header();
	int tweet_num;
	printf("Enter the tweet Number: ");
	scanf("%d", &tweet_num);
	int response = ReTweetToServer(tweet_num);
	if (response == 1) {
		int option = 0;
		while (option != 2 && option != 1) {
			system("cls");
			Header();
			printf(GREEN"You successfully retweeted tweet %d\n\n"RESET, tweet_num);
			printf("1.Retweet another tweet\n2.Back\n");
			scanf("%d", &option);
			if (option == 1)
				ReTweet();
			else if (option == 2)
				TimeLine();
		}
	}
	else if (response == 2) {
		int option = 0;
		while (option != 2 && option != 1) {
			system("cls");
			Header();
			printf(RED"There is no tweet with this number\n\n"RESET);
			printf("1.Try Again\n2.Back\n");
			scanf("%d", &option);
			if (option == 1)
				ReTweet();
			else if (option == 2)
				TimeLine();
		}
	}
	return;
}
void LikeOrComment() {
	char comments[1][1];
	int option = 0;
	while (option != 4 && option != 3 && option != 2 && option != 1) {
		system("cls");
		Header();
		printf("1.Like\n2.Comment\n3.See Comments\n4.Main Menu\n");
		scanf("%d", &option);
		if (option == 1)
			Like();
		else if (option == 2)
			Comment();
		else if (option == 3)
			ShowComments();
		else if (option == 4)
			MainMenu();
	}
	return;
}
void Like() {
	system("cls");
	Header();
	int tweet_num;
	printf("Enter the tweet Number: ");
	scanf("%d", &tweet_num);
	int response = LikeToServer(tweet_num);
	if (response == 1) {
		int option = 0;
		while (option != 2 && option != 1) {
			system("cls");
			Header();
			printf(GREEN"You successfully liked tweet %d\n\n"RESET, tweet_num);
			printf("1.Like another tweet\n2.Back\n");
			scanf("%d", &option);
			if (option == 1)
				Like();
			else if (option == 2)
				LikeOrComment();
		}
	}
	else if (response == 2) {
		int option = 0;
		while (option != 2 && option != 1) {
			system("cls");
			Header();
			printf(RED"There is no tweet with this number\n\n"RESET);
			printf("1.Try Again\n2.Back\n");
			scanf("%d", &option);
			if (option == 1)
				Like();
			else if (option == 2)
				LikeOrComment();
		}
	}
	return;
}
void Comment() {
	int tweet_num;
	char comment[200];
	system("cls");
	Header();
	printf("Enter the tweet number:  ");
	scanf("%d", &tweet_num);
	printf("Enter the comment:\n");
	scanf("%*c%[^\n]s", comment);
	int response = CommentToServer(tweet_num, comment);
	if (response == 1) {
		int option = 0;
		while (option != 2 && option != 1) {
			system("cls");
			Header();
			printf(GREEN"You successfully commented tweet %d\n\n"RESET, tweet_num);
			printf("1.Comment another tweet\n2.Back\n");
			scanf("%d", &option);
			if (option == 1)
				Comment();
			else if (option == 2)
				LikeOrComment();
		}
	}
	else if (response == 2) {
		int option = 0;
		while (option != 2 && option != 1) {
			system("cls");
			Header();
			printf(RED"There is no tweet with this number\n\n"RESET);
			printf("1.Try Again\n2.Back\n");
			scanf("%d", &option);
			if (option == 1)
				Comment();
			else if (option == 2)
				LikeOrComment();
		}
	}
	return;
}
void AdvanceSearch() {
	char username[50];
	system("cls");
	Header();
	printf("Enter the username\n");
	scanf("%s", username);
	char ** usernames = AdvanceSearchToServer(username);
	if (usernames == NULL) {
		int option = 3;
		while (option != 2 && option != 1) {
			system("cls");
			Header();
			printf(RED"There is no account with this username\n\n"RESET);
			printf("1.Try Again\n2.Back\n");
			scanf("%d", &option);
			if (option == 1)
				AdvanceSearch();
			else if (option == 2)
				MainMenu();
		}
	}
	else {
		int index = 0;
		int option = -1;
		while (option > index || option < 0) {
			system("cls");
			Header();
			printf(GREEN"The following usernames are found.\n"RESET);
			while (usernames[index] != NULL) {
				printf("%d. %s\n", index + 1, usernames[index]);
				index++;
			}
			printf("\n");
			printf("Choose one of the users or enter 0 to go back\n");
			scanf("%d", &option);
			if (option == 0)
				MainMenu();
			else if (option <= index || option > 0) {
				Search(usernames[option - 1]);
			}
		}
	}
	return;
}
void Search(char * username) {
	int follow_status;
	PROFILECOLLECTOR profile = SearchToServer(username, &follow_status);
	if (profile.error == 0) {
		int option = 3;
		while (option != 2 && option != 1) {
			system("cls");
			Header();
			printf(RED"There is no account with this username\n\n"RESET);
			printf("1.Try Again\n2.Back\n");
			scanf("%d", &option);
			if (option == 1)
				AdvanceSearch();
			else if (option == 2)
				MainMenu();
		}
	}
	else {
		int option = 3;
		while (option != 2 && option != 1) {
			system("cls");
			Header();
			printf("Username: ");
			PrintStr(profile.username);
			printf("\nbio: ");
			PrintStr(profile.bio);
			printf("\nfollowers: ");
			PrintStr(profile.followers);
			printf("   followings: ");
			PrintStr(profile.followings);
			printf("\nfollow status: ");
			PrintStr(profile.followstatus);
			printf("\n\n");
			int i = 0;
			while (profile.tweets[i].username != NULL) {
				PrintStr(profile.tweets[i].username);
				printf("\t");
				PrintStr(profile.tweets[i].id);
				printf("\n");
				PrintStr(profile.tweets[i].content);
				printf("\nlikes: ");
				PrintStr(profile.tweets[i].like);
				printf("  comments: %d\n\n", profile.tweets[i].comment);
				i++;
			}
			free(profile.tweets);
			printf("\n\n");
			if (CompareStr(profile.followstatus, "\"Followed\"")) {
				printf("1.UnFollow\n2.Back\n");
				scanf("%d", &option);
				if (option == 1)
					UnFollow(username);
				else if (option == 2)
					MainMenu();
			}
			else {
				printf("1.Follow\n2.Back\n");
				scanf("%d", &option);
				if (option == 1)
					Follow(username);
				else if (option == 2)
					MainMenu();
			}
		}
	}
	return;
}
void TweetSearch() {
	char word[50];
	system("cls");
	Header();
	printf("Enter the word\n");
	scanf("%s", word);
	TWEETCOLLECTOR * message_from_server = SearchTweetToServer(word);
	int option = 0, i = 0;
	if (CompareStr(message_from_server[0].id, "Error") == 1) {
		int option = 0;
		while (option != 1 && option != 2) {
			system("cls");
			Header();
			printf(RED"There is no tweet available\n\n"RESET);
			printf("1.Try again\n2.Back\n");
			scanf("%d", &option);
			if (option == 1)
				TweetSearch();
			else if (option == 2)
				MainMenu();
		}
	}
	else {
		while (option != 2 && option != 1) {
			system("cls");
			Header();
			while (message_from_server[i].username != NULL) {
				PrintStr(message_from_server[i].username);
				printf("\t");
				PrintStr(message_from_server[i].id);
				printf("\n");
				PrintStr(message_from_server[i].content);
				printf("\nlikes: ");
				PrintStr(message_from_server[i].like);
				printf("  comments: %d\n\n", message_from_server[i].comment);
				i++;
			}
			free(message_from_server);
			printf("1.Search another tweet\n2.Back\n");
			scanf("%d", &option);
			if (option == 1)
				TweetSearch();
			else if (option == 2)
				MainMenu();
		}
	}
	return;
}
void TweetProfile() {
	PROFILECOLLECTOR profile = ProfileToServer();
	int option = 0;
	while (option != 1) {
		system("cls");
		Header();
		printf("Username: ");
		PrintStr(profile.username);
		printf("\nbio: ");
		PrintStr(profile.bio);
		printf("\nfollowers: ");
		PrintStr(profile.followers);
		printf("   followings: ");
		PrintStr(profile.followings);
		printf("\n\n");
		int i = 0;
		while (profile.tweets[i].username != NULL) {
			PrintStr(profile.tweets[i].username);
			printf("\t");
			PrintStr(profile.tweets[i].id);
			printf("\n");
			PrintStr(profile.tweets[i].content);
			printf("\nlikes: ");
			PrintStr(profile.tweets[i].like);
			printf("  comments: %d\n\n", profile.tweets[i].comment);
			i++;
		}
		free(profile.tweets);
		printf("1.Back\n");
		scanf("%d", &option);
	}
	MainMenu();
	return;
}
void Follow(char * username) {
	int option = 0;
	FollowToServer(username);
	while (option != 2 && option != 1) {
		system("cls");
		Header();
		printf(GREEN"You successfully followed %s\n\n"RESET, username);
		printf("1.Search a user\n2.Back\n");
		scanf("%d", &option);
		if (option == 1)
			AdvanceSearch();
		else if (option == 2)
			MainMenu();
	}
}
void UnFollow(char * username) {
	int option = 0;
	UnFollowToServer(username);
	while (option != 2 && option != 1) {
		system("cls");
		Header();
		printf(GREEN"You successfully unfollowed %s\n\n"RESET, username);
		printf("1.Search a user\n2.Back\n");
		scanf("%d", &option);
		if (option == 1)
			AdvanceSearch();
		else if (option == 2)
			MainMenu();
	}
}
void SetBio() {
	char bio[50];
	system("cls");
	Header();
	printf("Enter the Bio:\n");
	scanf("%*c%[^\n]s", bio);
	int response = SetBioToServer(bio);
	if (response == 1) {
		int option = 0;
		while (option != 1) {
			system("cls");
			Header();
			printf(RED"You already have a bio\n\n"RESET);
			printf("1.Back\n");
			scanf("%d", &option);
		}
	}
	PersonalArea();
	return;
}
void ChangePassword() {
	char current_password[20];
	char new_password[20];
	system("cls");
	Header();
	printf("Enter the Current Password: ");
	scanf("%s", current_password);
	printf("Enter the New Password: ");
	scanf("%s", new_password);
	int response = ChangePasswordToServer(current_password, new_password);
	if (response == 1) {
		int option = 0;
		while (option != 1) {
			system("cls");
			Header();
			printf(GREEN"Your password has been changed successfuly\n\n"RESET);
			printf("1.Back\n");
			scanf("%d", &option);
			PersonalArea();
		}
	}
	else if (response == 2) {
		int option = 3;
		while (option != 2 && option != 1) {
			system("cls");
			Header();
			printf(RED"The passwors is incorrect or equal to current password\n"RESET);
			printf("1.Try Again\n2.Back\n");
			scanf("%d", &option);
			if (option == 1)
				ChangePassword();
			else if (option == 2)
				PersonalArea();
		}
	}
	return;
}
void PersonalArea() {
	int option = 0;
	while (option != 3 && option != 2 && option != 1) {
		system("cls");
		Header();
		printf("1.Set Bio\n2.Change Password\n3.Back\n");
		scanf("%d", &option);
		if (option == 1)
			SetBio();
		else if (option == 2)
			ChangePassword();
		else if (option == 3)
			MainMenu();
	}
	return;
}
void ShowComments() {
	system("cls");
	Header();
	printf("Enter the tweet number: ");
	int tweet_id;
	scanf("%d", &tweet_id);
	if (comments[tweet_id][0] == '-' && comments[tweet_id][1] == '1') {
		int option = 0;
		while (option != 2 && option != 1) {
			system("cls");
			Header();
			printf(RED"There is no tweet with this number\n\n"RESET);
			printf("1.Try Again\n2.Back\n");
			scanf("%d", &option);
			if (option == 1)
				ShowComments();
			else if (option == 2)
				LikeOrComment();
		}
	}
	else if (comments[tweet_id][0] != '0') {
		int option = 0;
		while (option != 2 && option != 1) {
			system("cls");
			Header();
			PrintStr(comments[tweet_id]);
			printf("\n\n");
			printf("1.See other tweets comments\n2.Back\n");
			scanf("%d", &option);
			if (option == 1)
				ShowComments();
			else if (option == 2)
				LikeOrComment();
		}
	}
	else {
		int option = 0;
		while (option != 2 && option != 1) {
			system("cls");
			Header();
			printf(RED"There is no comment for this tweet\n\n"RESET);
			printf("1.Try Again\n2.Back\n");
			scanf("%d", &option);
			if (option == 1)
				ShowComments();
			else if (option == 2)
				LikeOrComment();
		}
	}
	return;
}

//TRNASLATOR
int SignUpToServer(char username[], char password[]) {
	if (Initialize() == -1)
		return 0;
	char message_to_server[100];
	sprintf(message_to_server, "signup %s, %s\n", username, password);
	char * message_from_server = ServerRespond(message_to_server);
	if (strncmp(message_from_server, "{\"type\":\"Successful\",\"message\":\"\"}", 20) == 0) {
		free(message_from_server);
		return 1;
	}
	else if (strncmp(message_from_server, "{\"type\":\"Error\",\"message\":\"This username is already taken\"}", 30) == 0) {
		free(message_from_server);
		return 2;
	}
	return 0;
}
int LogInToServer(char username[], char password[]) {
	if (Initialize() == -1)
		return 0;
	char message_to_server[100];
	sprintf(message_to_server, "login %s, %s\n", username, password);
	char * message_from_server = ServerRespond(message_to_server);
	if (strncmp(message_from_server, "{\"type\":\"Token\"", 12) == 0) {
		int j = 27;
		for (int i = 0; i < 32; i++) {
			token[i] = message_from_server[j];
			j++;
		}
		token[32] = '\0';
		free(message_from_server);
		return 1;
	}
	else if (CompareStr(message_from_server, "{\"type\":\"Error\",\"message\":\"Incorrect password.\"}\n") == 1) {
		free(message_from_server);
		return 3;
	}
	else {
		free(message_from_server);
		return 2;
	}
}
void SendTweetToServer(char tweet[]) {
	if (Initialize() == -1)
		return;
	char message_to_server[3000];
	sprintf(message_to_server, "send tweet %s, %s\n", token, tweet);
	free(ServerRespond(message_to_server));
}
TWEETCOLLECTOR * RefreshToServer() {
	if (Initialize() == -1)
		return 0;
	TWEETCOLLECTOR * list = (TWEETCOLLECTOR *)malloc(sizeof(TWEETCOLLECTOR));
	char message_to_server[50];
	sprintf(message_to_server, "refresh %s\n", token);
	int can_initialize = Initialize();
	char * message_from_server = ServerRespond(message_to_server);
	if (CompareStr(message_from_server, "{\"type\":\"Successful\",\"message\":[]}\n") == 1) {
		list[0].id = "Error";
		free(message_from_server);
		return list;
	}
	int i = 0, counter = 1;
	while (message_from_server[i] != '[')
		i++;
	while (message_from_server[i] != ']') {
		int comment_counter = 0;
		int flag = 0; // ID
		while (message_from_server[i] != ',') {
			if (flag == 1) {
				list[counter - 1].id = CopyStr(message_from_server + i, ',', &i);
				i--;
			}
			if (message_from_server[i] == ':')
				flag = 1;
			i++;
		}
		i++;
		flag = 0; // USER NAME
		while (message_from_server[i] != ',') {
			if (flag == 1) {
				list[counter - 1].username = CopyStr(message_from_server + i, ',', &i);
				i--;
			}
			if (message_from_server[i] == ':')
				flag = 1;
			i++;
		}
		i++;
		flag = 0; //CONTENT
		while (message_from_server[i] != ',') {
			if (flag == 1) {
				list[counter - 1].content = CopyStr(message_from_server + i, ',', &i);
				i--;
			}
			if (message_from_server[i] == ':')
				flag = 1;
			i++;
		}
		flag = 0;
		int keep_i = i + 1; //COMMENTS
		while (message_from_server[i] != '{')
			i++;
		i++;
		list[counter - 1].comments = CopyStr(message_from_server + i, '}', &i);
		i = keep_i;
		while (message_from_server[i] != '}') {
			if (message_from_server[i] == ':')
				comment_counter++;
			i++;
		}
		list[counter - 1].comment = comment_counter - 1;
		int tweet_id = StrToInt(list[counter - 1].id);
		if (strlen(list[counter - 1].comments) < 3)
			comments[tweet_id][0] = '0';
		else
			CopyStrToStr(comments[tweet_id], list[counter - 1].comments);
		flag = 0;
		i++; //LIKES
		while (message_from_server[i] != '}') {
			if (flag == 1) {
				list[counter - 1].like = CopyStr(message_from_server + i, '}', &i);
				i--;
			}
			if (message_from_server[i] == ':')
				flag = 1;
			i++;
		}
		i++;
		if (message_from_server[i] == ']')
			break;
		i++;
		counter++;
		list = (TWEETCOLLECTOR *)realloc(list, counter * sizeof(TWEETCOLLECTOR));
	}
	free(message_from_server);
	list = (TWEETCOLLECTOR *)realloc(list, (counter + 1) * sizeof(TWEETCOLLECTOR));
	list[counter].username = NULL;
	return list;
}
int LikeToServer(int tweet_num) {
	if (Initialize() == -1)
		return 0;
	char message_to_server[100];
	sprintf(message_to_server, "like %s, %d\n", token, tweet_num);
	char * message_from_server = ServerRespond(message_to_server);
	if (CompareStr(message_from_server, "{\"type\":\"Error\",\"message\":\"Tweet with this id is not available.\"}\n") == 1) {
		free(message_from_server);
		return 2;
	}
	else {
		free(message_from_server);
		return 1;
	}
}
int CommentToServer(int tweet_num, char comment[]) {
	if (Initialize() == -1)
		return 0;
	char message_to_server[350];
	sprintf(message_to_server, "comment %s, %d, %s\n", token, tweet_num, comment);
	char * message_from_server = ServerRespond(message_to_server);
	if (CompareStr(message_from_server, "{\"type\":\"Error\",\"message\":\"Tweet with this id is not available.\"}\n") == 1) {
		free(message_from_server);
		return 2;
	}
	else {
		free(message_from_server);
		return 1;
	}
}
PROFILECOLLECTOR SearchToServer(char username[], int * follow_status) {
	if (Initialize() == -1)
		return;
	PROFILECOLLECTOR  profile;
	char message_to_server[100];
	sprintf(message_to_server, "search %s, %s\n", token, username);
	char * message_from_server = ServerRespond(message_to_server);
	if (CompareStr(message_from_server, "{\"type\":\"Error\",\"message\":\"This username is not valid.\"}\n") == 1) {
		free(message_from_server);
		profile.error = 0;
		return profile;
	}
	profile.error = 1;
	int i = 1;
	while (message_from_server[i] != '{')
		i++;

	int flag = 0;
	while (message_from_server[i] != ',') {
		if (flag == 1) {
			profile.username = CopyStr(message_from_server + i, ',', &i);
			i--;
		}
		if (message_from_server[i] == ':')
			flag = 1;
		i++;
	}
	i++;
	flag = 0;
	while (message_from_server[i] != ',') {
		if (flag == 1) {
			profile.bio = CopyStr(message_from_server + i, ',', &i);
			i--;
		}
		if (message_from_server[i] == ':')
			flag = 1;
		i++;
	}
	i++;
	flag = 0;
	while (message_from_server[i] != ',') {
		if (flag == 1) {
			profile.followers = CopyStr(message_from_server + i, ',', &i);
			i--;
		}
		if (message_from_server[i] == ':')
			flag = 1;
		i++;
	}
	i++;
	flag = 0;
	while (message_from_server[i] != ',') {
		if (flag == 1) {
			profile.followings = CopyStr(message_from_server + i, ',', &i);
			i--;
		}
		if (message_from_server[i] == ':')
			flag = 1;
		i++;
	}
	i++;
	flag = 0;
	while (message_from_server[i] != ',') {
		if (flag == 1) {
			profile.followstatus = CopyStr(message_from_server + i, ',', &i);
			i--;
		}
		if (message_from_server[i] == ':')
			flag = 1;
		i++;
	}
	i++;
	flag = 0;
	int counter = 1;
	profile.tweets = (TWEETCOLLECTOR *)malloc(sizeof(TWEETCOLLECTOR));
	while (message_from_server[i] != '[')
		i++;
	int while_flag = 0;
	while (message_from_server[i] != ']' && message_from_server[i + 1] != ']') {
		while_flag = 1;
		int comment_counter = 0;
		int flag = 0;
		while (message_from_server[i] != ',') {
			if (flag == 1) {
				profile.tweets[counter - 1].id = CopyStr(message_from_server + i, ',', &i);
				i--;
			}
			if (message_from_server[i] == ':')
				flag = 1;
			i++;
		}
		i++;
		flag = 0;
		while (message_from_server[i] != ',') {
			if (flag == 1) {
				profile.tweets[counter - 1].username = CopyStr(message_from_server + i, ',', &i);
				i--;
			}
			if (message_from_server[i] == ':')
				flag = 1;
			i++;
		}
		i++;
		flag = 0;
		while (message_from_server[i] != ',') {
			if (flag == 1) {
				profile.tweets[counter - 1].content = CopyStr(message_from_server + i, ',', &i);
				i--;
			}
			if (message_from_server[i] == ':')
				flag = 1;
			i++;
		}
		flag = 0;
		int keep_i = i + 1; //COMMENTS
		while (message_from_server[i] != '{')
			i++;
		i++;
		profile.tweets[counter - 1].comments = CopyStr(message_from_server + i, '}', &i);
		i = keep_i;
		while (message_from_server[i] != '}') {
			if (message_from_server[i] == ':')
				comment_counter++;
			i++;
		}
		profile.tweets[counter - 1].comment = comment_counter - 1;
		int tweet_id = StrToInt(profile.tweets[counter - 1].id);
		if (strlen(profile.tweets[counter - 1].comments) < 3)
			comments[tweet_id][0] = '0';
		else
			CopyStrToStr(comments[tweet_id], profile.tweets[counter - 1].comments);
		flag = 0;
		i++;
		while (message_from_server[i] != '}') {
			if (flag == 1) {
				profile.tweets[counter - 1].like = CopyStr(message_from_server + i, '}', &i);
				i--;
			}
			if (message_from_server[i] == ':')
				flag = 1;
			i++;
		}
		i++;
		if (message_from_server[i] == ']')
			break;
		i++;
		counter++;
		profile.tweets = (TWEETCOLLECTOR *)realloc(profile.tweets, counter * sizeof(TWEETCOLLECTOR));
	}
	free(message_from_server);
	if (while_flag == 1) {
		profile.tweets = (TWEETCOLLECTOR *)realloc(profile.tweets, (counter + 1) * sizeof(TWEETCOLLECTOR));
	}
	else
		counter--;
	profile.tweets[counter].username = NULL;
	return profile;
}
void FollowToServer(char username[]) {
	if (Initialize() == -1)
		return;
	char message_to_server[100];
	sprintf(message_to_server, "follow %s, %s\n", token, username);
	free(ServerRespond(message_to_server));
}
void UnFollowToServer(char username[]) {
	if (Initialize() == -1)
		return;
	char message_to_server[100];
	sprintf(message_to_server, "unfollow %s, %s\n", token, username);
	free(ServerRespond(message_to_server));
}
PROFILECOLLECTOR ProfileToServer() {
	if (Initialize() == -1)
		return;
	PROFILECOLLECTOR  profile;
	char message_to_server[100];
	sprintf(message_to_server, "profile %s\n", token);
	int i = 1;
	char * message_from_server = ServerRespond(message_to_server);
	while (message_from_server[i] != '{')
		i++;

	int flag = 0;
	while (message_from_server[i] != ',') {
		if (flag == 1) {
			profile.username = CopyStr(message_from_server + i, ',', &i);
			i--;
		}
		if (message_from_server[i] == ':')
			flag = 1;
		i++;
	}
	i++;
	flag = 0;
	while (message_from_server[i] != ',') {
		if (flag == 1) {
			profile.bio = CopyStr(message_from_server + i, ',', &i);
			i--;
		}
		if (message_from_server[i] == ':')
			flag = 1;
		i++;
	}
	i++;
	flag = 0;
	while (message_from_server[i] != ',') {
		if (flag == 1) {
			profile.followers = CopyStr(message_from_server + i, ',', &i);
			i--;
		}
		if (message_from_server[i] == ':')
			flag = 1;
		i++;
	}
	i++;
	flag = 0;
	while (message_from_server[i] != ',') {
		if (flag == 1) {
			profile.followings = CopyStr(message_from_server + i, ',', &i);
			i--;
		}
		if (message_from_server[i] == ':')
			flag = 1;
		i++;
	}
	i++;
	flag = 0;
	while (message_from_server[i] != ',') {
		if (flag == 1) {
			profile.followstatus = CopyStr(message_from_server + i, ',', &i);
			i--;
		}
		if (message_from_server[i] == ':')
			flag = 1;
		i++;
	}
	i++;
	flag = 0;
	int counter = 1;
	profile.tweets = (TWEETCOLLECTOR *)malloc(sizeof(TWEETCOLLECTOR));
	while (message_from_server[i] != '[')
		i++;
	int while_flag = 0;
	while (message_from_server[i] != ']' && message_from_server[i + 1] != ']') {
		while_flag = 1;
		int comment_counter = 0;
		int flag = 0;
		while (message_from_server[i] != ',') {
			if (flag == 1) {
				profile.tweets[counter - 1].id = CopyStr(message_from_server + i, ',', &i);
				i--;
			}
			if (message_from_server[i] == ':')
				flag = 1;
			i++;
		}
		i++;
		flag = 0;
		while (message_from_server[i] != ',') {
			if (flag == 1) {
				profile.tweets[counter - 1].username = CopyStr(message_from_server + i, ',', &i);
				i--;
			}
			if (message_from_server[i] == ':')
				flag = 1;
			i++;
		}
		i++;
		flag = 0;
		while (message_from_server[i] != ',') {
			if (flag == 1) {
				profile.tweets[counter - 1].content = CopyStr(message_from_server + i, ',', &i);
				i--;
			}
			if (message_from_server[i] == ':')
				flag = 1;
			i++;
		}
		flag = 0;
		int keep_i = i + 1; //COMMENTS
		while (message_from_server[i] != '{')
			i++;
		i++;
		profile.tweets[counter - 1].comments = CopyStr(message_from_server + i, '}', &i);
		i = keep_i;
		while (message_from_server[i] != '}') {
			if (message_from_server[i] == ':')
				comment_counter++;
			i++;
		}
		profile.tweets[counter - 1].comment = comment_counter - 1;
		int tweet_id = StrToInt(profile.tweets[counter - 1].id);
		if (strlen(profile.tweets[counter - 1].comments) < 3)
			comments[tweet_id][0] = '0';
		else
			CopyStrToStr(comments[tweet_id], profile.tweets[counter - 1].comments);
		flag = 0;
		i++;
		while (message_from_server[i] != '}') {
			if (flag == 1) {
				profile.tweets[counter - 1].like = CopyStr(message_from_server + i, '}', &i);
				i--;
			}
			if (message_from_server[i] == ':')
				flag = 1;
			i++;
		}
		i++;
		if (message_from_server[i] == ']')
			break;
		i++;
		counter++;
		profile.tweets = (TWEETCOLLECTOR *)realloc(profile.tweets, counter * sizeof(TWEETCOLLECTOR));
	}
	free(message_from_server);
	if (while_flag == 1) {
		profile.tweets = (TWEETCOLLECTOR *)realloc(profile.tweets, (counter + 1) * sizeof(TWEETCOLLECTOR));
	}
	else
		counter--;
	profile.tweets[counter].username = NULL;
	return profile;
}
int ChangePasswordToServer(char current_password[], char new_password[]) {
	if (Initialize() == -1)
		return 0;
	char message_to_server[100];
	sprintf(message_to_server, "change password %s, %s, %s\n", token, current_password, new_password);
	char * message_from_server = ServerRespond(message_to_server);
	if (CompareStr(message_from_server, "{\"type\":\"Successful\",\"message\":\"Password is changed successfully.\"}\n") == 1) {
		free(message_from_server);
		return 1;
	}
	else {
		free(message_from_server);
		return 2;
	}
}
void LogOutToServer() {
	if (Initialize() == -1)
		return;
	char message_to_server[50];
	sprintf(message_to_server, "logout %s\n", token);
	free(ServerRespond(message_to_server));
}
int SetBioToServer(char bio[]) {
	if (Initialize() == -1)
		return 0;
	char message_to_server[100];
	sprintf(message_to_server, "set bio %s, %s\n", token, bio);
	char * message_from_server = ServerRespond(message_to_server);
	if (CompareStr(message_from_server, "{\"type\":\"Successful\",\"message\":\"Bio is updated successfully.\"}\n") == 0) {
		free(message_from_server);
		return 1;
	}
	free(message_from_server);
	return 0;
}
int DeleteTweetToServer(int tweet_num) {
	if (Initialize() == -1)
		return 0;
	char message_to_server[100];
	sprintf(message_to_server, "delete tweet %s, %d\n", token, tweet_num);
	char * message_from_server = ServerRespond(message_to_server);
	if (CompareStr(message_from_server, "{\"type\":\"Error\",\"message\":\"Tweet with this id is not available.\"}\n") == 1) {
		free(message_from_server);
		return 2;
	}
	if (strncmp(message_from_server, "{\"type\":\"Error\",\"message\":\"Tweet blongs to someone else.\"}\n", 50) == 0) {
		free(message_from_server);
		return 3;
	}
	free(message_from_server);
	return 1;
}
int ReTweetToServer(int tweet_num) {
	if (Initialize() == -1)
		return 0;
	char message_to_server[100];
	sprintf(message_to_server, "retweet %s, %d\n", token, tweet_num);
	char * message_from_server = ServerRespond(message_to_server);
	if (CompareStr(message_from_server, "{\"type\":\"Error\",\"message\":\"Tweet with this id is not available.\"}\n") == 1) {
		free(message_from_server);
		return 2;
	}
	free(message_from_server);
	return 1;
}
char ** AdvanceSearchToServer(char * username) {
	if (Initialize() == -1)
		return 0;
	char message_to_server[100];
	sprintf(message_to_server, "advance search %s, %s\n", token, username);
	char * message_from_server = ServerRespond(message_to_server);
	if (strncmp(message_from_server, "{\"type\":\"Error\",\"message\":\"This username is not valid.\"}\n", 30) == 0)
		return NULL;
	char ** users = (char **)calloc(sizeof(char *), 100);
	int message_from_server_index = 28, users_index = 0, user_index = 0;
	while (message_from_server[message_from_server_index] != '}') {
		users[users_index] = (char *)calloc(sizeof(char), 50);
		while (message_from_server[message_from_server_index] != '}' && message_from_server[message_from_server_index] != ',') {
			users[users_index][user_index] = message_from_server[message_from_server_index];
			user_index++;
			message_from_server_index++;
		}
		users[users_index][user_index - 1] = '\0';
		user_index = 0;
		users_index++;
		if (message_from_server[message_from_server_index] != '}')
			message_from_server_index += 2;
	}
	users[users_index] = NULL;
	return users;
}
TWEETCOLLECTOR * SearchTweetToServer(char * word) {
	if (Initialize() == -1)
		return 0;
	TWEETCOLLECTOR * list = (TWEETCOLLECTOR *)malloc(sizeof(TWEETCOLLECTOR));
	char message_to_server[200];
	sprintf(message_to_server, "tweet search %s, %s\n", token, word);
	int can_initialize = Initialize();
	char * message_from_server = ServerRespond(message_to_server);
	if (strncmp(message_from_server, "{\"type\":\"Error\",\"message\":\"No tweet is found.\"}\n", 45) == 0) {
		list[0].id = "Error";
		free(message_from_server);
		return list;
	}
	int i = 0, counter = 1;
	while (strncmp(&message_from_server[i], "\"message\":", 10) != 0)
		i++;
	i += 10;
	while (message_from_server[i] != '\n') {
		int comment_counter = 0;
		int flag = 0; // ID
		while (message_from_server[i] != ',') {
			if (flag == 1) {
				list[counter - 1].id = CopyStr(message_from_server + i, ',', &i);
				i--;
			}
			if (message_from_server[i] == ':')
				flag = 1;
			i++;
		}
		i++;
		flag = 0; // USER NAME
		while (message_from_server[i] != ',') {
			if (flag == 1) {
				list[counter - 1].username = CopyStr(message_from_server + i, ',', &i);
				i--;
			}
			if (message_from_server[i] == ':')
				flag = 1;
			i++;
		}
		i++;
		flag = 0; //CONTENT
		while (message_from_server[i] != ',') {
			if (flag == 1) {
				list[counter - 1].content = CopyStr(message_from_server + i, ',', &i);
				i--;
			}
			if (message_from_server[i] == ':')
				flag = 1;
			i++;
		}
		flag = 0;
		int keep_i = i + 1; //COMMENTS
		while (message_from_server[i] != '{')
			i++;
		i++;
		list[counter - 1].comments = CopyStr(message_from_server + i, '}', &i);
		i = keep_i;
		while (message_from_server[i] != '}') {
			if (message_from_server[i] == ':')
				comment_counter++;
			i++;
		}
		list[counter - 1].comment = comment_counter - 1;
		int tweet_id = StrToInt(list[counter - 1].id);
		if (strlen(list[counter - 1].comments) < 3)
			comments[tweet_id][0] = '0';
		else
			CopyStrToStr(comments[tweet_id], list[counter - 1].comments);
		flag = 0;
		i++; //LIKES
		while (message_from_server[i] != '}') {
			if (flag == 1) {
				list[counter - 1].like = CopyStr(message_from_server + i, '}', &i);
				i--;
			}
			if (message_from_server[i] == ':')
				flag = 1;
			i++;
		}
		i++;
		if (message_from_server[i] == ']')
			break;
		i++;
		counter++;
		list = (TWEETCOLLECTOR *)realloc(list, counter * sizeof(TWEETCOLLECTOR));
	}
	free(message_from_server);
	list = (TWEETCOLLECTOR *)realloc(list, (counter + 1) * sizeof(TWEETCOLLECTOR));
	list[counter - 1].username = NULL;
	return list;
}

//BACK END
char * ServerRespond(char * message_to_server) {
	char * buffer = (char *)malloc(1000000);
	memset(buffer, '0', 1000000);

	int client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket == INVALID_SOCKET)
		return buffer;

	int can_connect = connect(client_socket, (struct sockaddr *) &server, sizeof(server));
	if (can_connect != 0)
		return buffer;
	send(client_socket, message_to_server, strlen(message_to_server), 0);
	recv(client_socket, buffer, 999999, 0);
	closesocket(client_socket);
	return buffer;
}
int Initialize() {
	WSADATA wsadata;
	int wsaStartUp = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (wsaStartUp != 0) {
		printf("Error");
		return -1;
	}
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_port = htons(PORT);
	return 0;
}

//STRING FUNCTIONS
char * CopyStr(char * input, char mode, int * counter) {
	char * output = (char *)malloc(sizeof(char));
	int i = 1;
	while (input[i - 1] != mode) {
		output[i - 1] = input[i - 1];
		i++;
		output = (char*)realloc(output, sizeof(char)*i);
	}
	*counter += (i - 1);
	output[i - 1] = '\0';
	return output;
}
int CompareStr(char str1[], char str2[]) {
	int i = 0;
	while (str1[i] != '0' && str1[i] != '\0') {
		if (str1[i] != str2[i])
			return 0;
		i++;
	}
	return 1;
}
void PrintStr(char str[]) {
	int i = 0;
	while (str[i] != '\0') {
		if (str[i] == ',')
			printf("\n");
		else if (str[i] != '\"')
			printf("%c", str[i]);
		i++;
	}
}
char * StringGetter()
{
	char* temp = (char *)calloc(1, sizeof(char));
	temp[0] = 'a';
	int counter = 1;
	while (1) {
		temp[counter - 1] = getchar();
		if (temp[counter - 1] == '\n')
			break;
		counter++;
		temp = (char)realloc(temp, sizeof(char)*counter);
	}
	return temp;
}
int StrToInt(char * str) {
	int i = 0, num = 0;
	while (str[i] != '\0') {
		if (str[i] <= 57 && str[i] >= 48) {
			num += (str[i] - 48);
			num *= 10;
		}
		i++;
	}
	return (num / 10);
}
void CopyStrToStr(char * str1, char * str2) {
	int i = 0;
	while (str2[i] != '\0') {
		str1[i] = str2[i];
		i++;
	}
}
