// Used in the MODE_CLASS member class
#include <string>
class Client;

struct Noun
{
	virtual std::string greet() = 0;

	virtual int setFlag(char flag, Noun *obj, bool active, Client &caller) = 0;
};

struct StrNoun : public Noun
{
	StrNoun() : content() {}
	StrNoun(std::string content) : content(content) {}
	StrNoun(const StrNoun &cpy) : content(cpy.content) {}
	StrNoun	&operator()(std::string _content);
	StrNoun	&operator=(const StrNoun &cpy);
	virtual ~StrNoun();
	std::string	content;

	virtual std::string greet();
	virtual int setFlag(char flag, Noun *obj, bool active, Client &caller);
};

int	flag_val(std::string alphabet, char flag);


template <class flagbytes>
class RuleSetter
{
	private:
		const std::string	_alphabet;
	public:
		RuleSetter(std::string alphabet) : _alphabet(alphabet) {}
		RuleSetter(const RuleSetter &cpy) : _alphabet(cpy._alphabet) {}

		std::string	getAlphabet() const
		{
			return (_alphabet);
		}

		void	operator()(flagbytes &toModify, char toAdd, bool active)
		{
			int	flag_value = flag_val(_alphabet, toAdd);
			if (active)
				toModify |= flag_value;
			else
				toModify &= ~flag_value;
		}
};

typedef struct	s_names
{
	std::string	nick;
	std::string	user;
	std::string	host;

	bool	operator<(const s_names	&other) const;
	s_names(const Client &request);
	s_names(const std::string &pattern);
	class WrongFormatException : public std::exception
	{
		const virtual char *what() const throw();
	};
} t_names;
