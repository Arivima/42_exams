# include <stdio.h>
# include <unistd.h>
# include <stdlib.h>
# include <string.h>
# include <sys/wait.h>
# define ERR -1
// ./microshell /bin/ls "|" /usr/bin/grep microshell ";" /bin/echo i love my microshell

typedef struct 	s_cmd
{
	char			**cmd_arg;
	int				pipe;
	int				fd[2];
	struct s_cmd	*next;
}				t_cmd;

int	ft_strlen(char *s)
{
	int	i = 0;
	while (s[i])
		i++;
	return (i);
}

int	ft_error(char *s, char *cmd)
{
	write(2, &s, ft_strlen(s));
	if (cmd)
		write(2, &cmd, ft_strlen(cmd));
	write(2, "\n", 1);
	return (ERR);
}

int	ft_exit(void)
{
	ft_error("error: fatal", NULL);
	exit(1);
}

void	ft_free(t_cmd *cmd)
{
	t_cmd	*tmp;

	while (cmd)
	{
		tmp = cmd->next;
		free(cmd->cmd_arg);
		cmd->cmd_arg = NULL;
		free(cmd);
		cmd = tmp;
	}
}

t_cmd	*new_elem(int ac, char **av, int *i)
{
	t_cmd	*elem;
	int		j;

	elem = (t_cmd *) malloc(sizeof(t_cmd));
	if (!elem)
		ft_exit();
	elem->fd[0] = 0;
	elem->fd[1] = 0;
	elem->pipe = 0;
	elem->next = NULL;
	elem->cmd_arg = (char **) malloc(sizeof(char *) * ac);
	if (!elem->cmd_arg)
		ft_exit();
	j = 0;
	while (av[*i] && strcmp(av[*i], "|") && strcmp(av[*i], ";"))
		elem->cmd_arg[j++] = av[(*i)++];
	elem->cmd_arg[j] = NULL;
	if (av[*i] && !strcmp(av[*i], "|"))
		elem->pipe = 1;
	return (elem);
}

void	print_cmds(t_cmd *cmd)
{
	t_cmd	*tmp;
	int		i;

	tmp = cmd;
	printf("Printing commands\n");
	while (tmp)
	{
		printf("New element\n");
		i = 0;
		while (tmp->cmd_arg[i])
		{
			printf("cmd->arg[%d] = %s\n", i, tmp->cmd_arg[i]);
			i++;
		}
		printf("cmd->pipe = %d\n", tmp->pipe);
		printf("cmd->fd[0] = %d\n", tmp->fd[0]);
		printf("cmd->fd[1] = %d\n", tmp->fd[1]);
		printf("cmd->next = %p\n", tmp->next);
		tmp = tmp->next;
	}
}

int	cd(char **cmd_arg)
{
	if (cmd_arg[1] == NULL || cmd_arg[2])
		return (ft_error("error: cd: bad arguments", NULL));
	if (chdir(cmd_arg[1]))
		return (ft_error("error: cd: cannot change directory to ", cmd_arg[1]));
	return (0);
}

int	execute(t_cmd *head, char **env)
{
	t_cmd *cmd;

	cmd = head;
	while (cmd)
	{
		if (cmd->pipe)
		{
			int pip[2];
			if (pipe(pip) == ERR)
				ft_exit();
			cmd->fd[1] = pip[1];
			cmd->next->fd[0] = pip[0];
		}

		int pid;
		if (!strcmp(cmd->cmd_arg[0], "cd"))
			cd(cmd->cmd_arg);
		pid = fork();
		if (pid < 0)
			ft_exit();
		if (pid == 0)
		{
			if (cmd->fd[0])
				dup2(cmd->fd[0], 0);
			if (cmd->fd[1])
				dup2(cmd->fd[1], 1);
			execve(cmd->cmd_arg[0], cmd->cmd_arg, env);
			ft_error("error: cannot execute", cmd->cmd_arg[0]);
			exit(1);
		}
		waitpid(pid, 0, 0);
		if (cmd->fd[1])
			close(cmd->fd[1]);
		if (cmd->fd[0])
			close(cmd->fd[0]);

		cmd = cmd->next;
	}

	return (0);
}

int	main(int ac, char **av, char **env)
{
	t_cmd	*cmd;
	t_cmd	*head;
	int		i = 1;

	cmd = NULL;
	while (av[i])
	{
		if (!strcmp(av[i], ";") || !strcmp(av[i], "|"))
		{
			i++;
			continue ;
		}
		if (!cmd)
		{
			cmd = new_elem(ac, av, &i);
			head = cmd;
		}
		else
		{
			cmd->next = new_elem(ac, av, &i);
			cmd = cmd->next;
		}
	}
	print_cmds(head);
	execute(head, env);
	ft_free(head);
	return (0);
}
