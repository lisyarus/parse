#pragma once

#include <boost/utility/string_ref.hpp>

#include <memory>
#include <functional>
#include <iostream>

namespace lexer
{

	struct token_t
	{
		std::string::const_iterator begin, end;
	};

	struct lexer_t
	{
		lexer_t ( );
		~ lexer_t ( );

		typedef std::function<void(token_t)> callback_t;

		void add_rule (boost::string_ref rule, callback_t callback);

		void process (std::istream & is) const;
		void finish ( );

	private:
		struct impl_t;
		std::unique_ptr<impl_t> pimpl_;
	};

}
