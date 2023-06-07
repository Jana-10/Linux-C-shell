#include "headers.h"

int flag(char *list[50], int len)
{
	if (len == 1)
		return 0;
	int ret = 0;
	int i;
	for (i = 1; i < len; i++)
	{
		if (strcmp(list[i], "-l") == 0)
			ret += 2;
		else if (strcmp(list[i], "-a") == 0)
			ret += 1;
		else if (strcmp(list[i], "-la") == 0)
			return 3;
		else if (strcmp(list[i], "-al") == 0)
			return 3;
	}
	return ret;
}

int parsePath(char *list[50], int len)
{
	int i;
	for (i = 1; i < len; i++)
	{
		if (list[i][0] != '-' && list[i][0] != '&')
		{
			return i;
		}
	}
	return -1;
}

void ls_handler(char *tokens[], int no)
{
	struct dirent *de;
	DIR *dr = NULL;
	int flg = 0;
	for (int i = 1; i < no; i++)
	{
		if (tokens[i][0] != '-' && tokens[i][0] != '&')
		{
			flg = 1;
			dr = opendir(tokens[i]);
			break;
		}
	}
	if (flg == 0)
		dr = opendir(".");

	char *file[5000];
	if (dr == NULL)
	{
		printf("Could not open file directory");
		return;
	}
	int noFile = 0;

	long int blksize = 0;
	int info = flag(tokens, no);

	while ((de = readdir(dr)) != NULL)
	{
		file[noFile++] = de->d_name;

		if (info == 0)
		{
			if (strcmp(".", de->d_name) != 0 && strcmp("..", de->d_name) != 0)
				printf("%s\n", file[noFile - 1]);
		}
		else if (info == 1)
			printf("%s\n", file[noFile - 1]);

		else if ((info == 2 && strcmp(".", de->d_name) != 0 && strcmp("..", de->d_name) != 0) || info == 3)
		{
			struct stat fileStat;
			struct passwd *pwd;
			char date[12];
			int address = parsePath(tokens, no);
			if (address > 0)
			{
				char path1[100];
				sprintf(path1, "%s%s", tokens[address], file[noFile - 1]);
				if (stat(path1, &fileStat) < 0)
					return;
			}
			else
			{
				if (stat(file[noFile - 1], &fileStat) < 0)
					return;
			}

			printf((S_ISDIR(fileStat.st_mode)) ? "d" : "-");
			printf((fileStat.st_mode & S_IRUSR) ? "r" : "-");
			printf((fileStat.st_mode & S_IWUSR) ? "w" : "-");
			printf((fileStat.st_mode & S_IXUSR) ? "x" : "-");
			printf((fileStat.st_mode & S_IRGRP) ? "r" : "-");
			printf((fileStat.st_mode & S_IWGRP) ? "w" : "-");
			printf((fileStat.st_mode & S_IXGRP) ? "x" : "-");
			printf((fileStat.st_mode & S_IROTH) ? "r" : "-");
			printf((fileStat.st_mode & S_IWOTH) ? "w" : "-");
			printf((fileStat.st_mode & S_IXOTH) ? "x" : "-");
			printf(" %ld ", fileStat.st_nlink);

			if ((pwd = getpwuid(fileStat.st_uid)) != NULL)
				printf(" %-8.8s", pwd->pw_name);
			else
				printf(" %-8d", fileStat.st_uid);

			if ((pwd = getpwuid(fileStat.st_gid)) != NULL)
				printf(" %-8.8s", pwd->pw_name);
			else
				printf(" %-8d", fileStat.st_gid);

			printf(" %8ld ", fileStat.st_size);
			strftime(date, 20, "%b %d %H:%M ", localtime(&(fileStat.st_mtime)));
			printf(" %s ", date);
			blksize += fileStat.st_blocks;
			printf("%s\n", file[noFile - 1]);
		}
	}
	if (info >= 2)
	{
		blksize /= 2;
		printf("total: %ld\n", blksize);
		blksize = 0;
		printf("\n");
	}
	printf("\n");
	closedir(dr);
}