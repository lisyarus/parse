#include <lexer/lexer.hpp>

#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

#include <vector>
#include <sstream>
#include <stdexcept>

namespace
{

	struct bool_guard_t
	{
		bool_guard_t (bool & b)
			: b_(b)
		{
			b_ = true;
		}

		~ bool_guard_t ( )
		{
			b_ = false;
		}

	private:
		bool & b_;
	};

}

namespace lexer
{

	struct lexer_t::impl_t
	{
		std::vector<std::pair<boost::regex, lexer_t::callback_t>> rules;
		bool processing;

		impl_t ( )
			: processing(false)
		{ }
	};

	lexer_t::lexer_t ( )
		: pimpl_(new impl_t)
	{ }

	lexer_t::~lexer_t ( )
	{ }

	void lexer_t::add_rule (boost::string_ref rule, callback_t callback)
	{
		pimpl_->rules.push_back(std::make_pair(boost::regex(rule.data()), std::move(callback)));
	}

	void lexer_t::process (std::istream & is) const
	{
		std::string data;
		{
			std::ostringstream oss;
			oss << is.rdbuf();
			data = oss.str();
		}
		std::string::const_iterator pos = data.cbegin();

		bool_guard_t processing_guard(pimpl_->processing);

		while (pos != data.cend() && pimpl_->processing)
		{
			boost::smatch m;

			bool found = false;
			for (auto const & rule : pimpl_->rules)
			{
				if (boost::regex_search(pos, data.cend(), m, rule.first) && !m.empty() && m[0].first == pos)
				{
					rule.second(token_t{m[0].first, m[0].second});
					pos = m[0].second;
					found = true;
					break;
				}
			}

			if (!found)
			{
				throw std::runtime_error("unknown token at position " + boost::lexical_cast<std::string>(pos - data.cbegin()));
			}
		}
	}

	void lexer_t::finish ( )
	{
		pimpl_->processing = false;
	}

}
