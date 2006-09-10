/* 
    fi_utils.c: all utility/misc functions

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2, or (at
    your option) any later version.

    This program is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. */

#include "common.h"

static tcflag_t old_lflag;
static cc_t old_vtime;
extern int errno;

struct termios term;

void
get_terminal_attributes (void)
{
  if (tcgetattr (STDIN_FILENO, &term) < 0)
    {
      perror ("tcgetattr()");
      exit (EXIT_FAILURE);
    }
  old_lflag = term.c_lflag;
  old_vtime = term.c_cc[VTIME];
}

void
set_terminal_attributes (void)
{
  term.c_lflag = old_lflag;
  term.c_cc[VTIME] = old_vtime;
  if (tcsetattr (STDIN_FILENO, TCSANOW, &term) < 0)
    {
      perror ("tcsetattr()");
      exit (EXIT_FAILURE);
    }
}

/* Trims the leading and trailing white spaces of a string  */
char *
stripwhite (char *string)
{
  register char *s, *t;

  for (s = string; whitespace (*s); s++)
    ;

  if (*s == 0)
    return s;

  t = s + strlen (s) - 1;
  while (t > s && whitespace (*t))
    t--;
  *++t = '\0';

  return s;
}


char *
get_token (char **line)
{
  char *command;
  while (1)
    {
      command = (char *) strsep (line, " ");
      if (!command)
	break;
      if (*(command))
	break;
    }
  return command;
}

char *
get_token_with_strdelim (char **line, char *delim)
{
  char *token = (char *) NULL;
  char *tmp_str = (char *) NULL;
  int token_length = 0;

  if (!(*line) || !delim)
    return token;

  tmp_str = strstr (*line, delim);
  if (tmp_str)
    token_length = strlen (*line) - strlen (tmp_str);
  else
    token_length = strlen (*line);

  token = (char *) malloc (sizeof (char) * (token_length + 1));
  strncpy (token, *line, token_length);
  token[token_length] = (char) NULL;
  *line = tmp_str;

  return token;
}

char *
get_token_with_strdelim_i (char **line, char *delim)
{
  char *token = (char *) NULL;
  char *tmp_str = (char *) NULL;
  int token_length = 0;

  if (!(*line) || !delim)
    return token;

  tmp_str = strcasestr (*line, delim);
  if (tmp_str)
    token_length = strlen (*line) - strlen (tmp_str);
  else
    token_length = strlen (*line);

  token = (char *) malloc (sizeof (char) * (token_length + 1));
  strncpy (token, *line, token_length);
  token[token_length] = (char) NULL;
  *line = tmp_str;

  return token;
}

char *
get_home_directory (void)
{
  struct passwd *current_passwd;
  char * username = "unknownuser";
  char * dir;
  char *tmpdir;

  current_passwd = getpwuid(getuid());
  if (current_passwd != NULL) {
    if (current_passwd->pw_dir != NULL &&
        access(current_passwd->pw_dir, R_OK | W_OK | X_OK) == 0)
      return strdup(current_passwd->pw_dir);
    if (current_passwd->pw_name != NULL)
      username = current_passwd->pw_name;
  }

  tmpdir = getenv("TMPDIR");
  if (tmpdir == NULL)
    tmpdir = "/tmp";
  dir = malloc (strlen (tmpdir) + 1 + sizeof("freeipmi-") - 1 +
    strlen (username) + 1);
  if (dir == NULL)
    return NULL;
  sprintf (dir, "%s/freeipmi-%s", tmpdir, username);
  if (mkdir (dir, 0700) < 0 && errno != EEXIST) {
    free(dir);
    return NULL;
  }
  return dir;
}

char *
get_config_directory (void)
{
  char *config_directory;
  char *homedir;
  int length;

  homedir = get_home_directory();
  if (homedir == NULL)
    return NULL;

  length = strlen (homedir) + strlen (FI_CONFIG_DIRECTORY) + 2;

  config_directory = (char *) calloc (length, sizeof (char));
  sprintf (config_directory, "%s/" FI_CONFIG_DIRECTORY, homedir);

  free(homedir);

  return config_directory;
}

char *
get_config_filename (void)
{
  char *config_filename;
  char *homedir;
  int length;

  homedir = get_home_directory();
  if (homedir == NULL)
    return NULL;

  length = strlen (homedir) + strlen (FI_CONFIG_FILE) + 2;

  config_filename = (char *) calloc (length, sizeof (char));
  sprintf (config_filename, "%s/" FI_CONFIG_FILE, homedir);

  free(homedir);

  return config_filename;
}


char *
get_global_extensions_directory (void)
{
  char *global_extensions_directory = NULL;
  
  asprintf (&global_extensions_directory, "%s",
	    FI_GLOBAL_EXTENSIONS_DIRECTORY);
  
  return global_extensions_directory;
}

char *
get_local_extensions_directory (void)
{
  char *local_extensions_directory = NULL;
  char *homedir;

  homedir = get_home_directory();
  if (homedir == NULL)
    return NULL;

  asprintf (&local_extensions_directory, 
	    "%s/%s", 
		homedir,
	    FI_LOCAL_EXTENSIONS_DIRECTORY);

  free(homedir);
  
  return local_extensions_directory;
}

int 
fi_load (char *filename)
{
  struct stat buf;
  
  char *ext_filename = NULL;
  char *ext_directory = NULL;
  
  ext_filename = strdup (filename);
  if (stat (ext_filename, &buf) == 0)
    {
      gh_eval_file_with_catch (ext_filename, fish_exception_handler);
      if (ext_filename)
	free (ext_filename);
      return 0;
    }
  if (ext_filename)
    free (ext_filename);
  
  ext_directory = get_local_extensions_directory ();
  asprintf (&ext_filename, "%s/%s", ext_directory, filename);
  if (stat (ext_filename, &buf) == 0)
    {
      gh_eval_file_with_catch (ext_filename, fish_exception_handler);
      if (ext_directory)
	free (ext_directory);
      if (ext_filename)
	free (ext_filename);
      return 0;
    }
  if (ext_directory)
    free (ext_directory);
  if (ext_filename)
    free (ext_filename);
  
  ext_directory = get_global_extensions_directory ();
  asprintf (&ext_filename, "%s/%s", ext_directory, filename);
  if (stat (ext_filename, &buf) == 0)
    {
      gh_eval_file_with_catch (ext_filename, fish_exception_handler);
      if (ext_directory)
	free (ext_directory);
      if (ext_filename)
	free (ext_filename);
      return 0;
    }
  if (ext_directory)
    free (ext_directory);
  if (ext_filename)
    free (ext_filename);
  
  return (-1);
}

char *
fi_getline (FILE *fp)
{
  char *line_string = NULL;
  char *line_start_ptr = NULL;
  size_t n = 0;
  int retval = 0;
  char *line = NULL;
  
  if (fp == NULL)
    return NULL;
  
  while (1)
    {
      if (line_string)
	{
	  free (line_string);
	  line_string = NULL;
	  line_start_ptr = NULL;
	  n = 0;
	}
      
      retval = getline (&line_string, &n, fp);
      if (retval == -1)
	break;
      
/*       printf ("%s:%d: line_string: [%s]\n", __FILE__, __LINE__, line_string); */
      
      line_start_ptr = stripwhite (line_string);
      
      if (strcmp (line_start_ptr, "\n") == 0 || 
	  strcmp (line_start_ptr, "\r\n") == 0)
	continue;
      
      if (line_start_ptr[0] != COMMENT_CHAR)
	break;
    }
  
  if (line_start_ptr)
    line = strdup (line_start_ptr);
  
  if (line_string)
    free (line_string);
  
  if (line)
    {
      if (line[strlen(line) - 1] == '\n')
	line[strlen(line) - 1] = 0;
      
      if (line[strlen(line) - 1] == '\r')
	line[strlen(line) - 1] = 0;
    }
  
  return line;
}


char *
fi_get_value (char *line)
{
  char *token_start_ptr = NULL;
  
  token_start_ptr = strchr (line,  ' ');
  if (token_start_ptr == NULL)
    return NULL;
  
  return strdup (stripwhite (token_start_ptr));
}

int 
is_valid_ip (char *ip)
{
  struct in_addr addr;
  return inet_aton (ip, &addr);
}

int 
is_valid_mac_address (char *mac_address)
{
  int i;
  
  for (i = 0; mac_address[i]; i++)
    {
      if (i >= 17)
	return 0;
      
      if ((i + 1) % 3 == 0)
	{
	  if (mac_address[i] == ':')
	    continue;
	  return 0;
	}
      
      if (!isxdigit (mac_address[i]))
	return 0;
    }
  
  if (i != 17)
    return 0;
  
  return 1;
}

