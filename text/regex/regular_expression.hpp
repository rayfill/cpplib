#ifndef CPP_LIB_TEXT_REGEX_REGULAR_EXPRESSIONS_HPP_
#define CPP_LIB_TEXT_REGEX_REGULAR_EXPRESSIONS_HPP_

#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <cassert>

namespace text 
{
	namespace regex
	{
		template <typename char_t>
		class acceptor
		{
		public:
			typedef char_t char_type;

		protected:
			virtual bool is_accept(char_type ch) const = 0;

		public:
			virtual ~acceptor()
			{}

			bool operator()(char_type ch) const
			{
				return is_accept(ch);
			}
		};

		template <typename char_t>
		class equal_acceptor : public acceptor<char_t>
		{
		public:
			typedef acceptor<char_t> base_type;
			typedef typename base_type::char_type char_type;

		private:
			char_type accept_character;

			equal_acceptor();

		protected:
			virtual bool is_accept(char_type ch) const
			{
				return accept_character == ch;
			}

		public:
			equal_acceptor(char_type ch):
				base_type(),
				accept_character(ch)
			{}

			equal_acceptor(const equal_acceptor& source):
				base_type(source),
				accept_character(source.accept_character)
			{}

			virtual ~equal_acceptor()
			{}
		};

		template <typename char_t>
		class node
		{
		public:
			typedef char_t char_type;
			typedef acceptor<char_type> acceptor_type;
			typedef node node_type;

		protected:
			virtual void add_entry_impl(acceptor_type* acceptor,
										node* node) = 0;

			virtual node_type* get_transition_impl(char_type ch) const = 0;

			virtual bool is_accept_impl(char_type ch) const = 0;

			virtual void set_epsilon_impl(node* epsilon) = 0;

			virtual node* get_epsilon_impl() const = 0;

		public:
			node()
			{}

			virtual ~node() {}

			void add_entry(acceptor_type* acceptor, node_type* node)
			{
				add_entry_impl(acceptor, node);
			}

			node_type* get_transition(char_type ch) const
			{
				return get_transition_impl(ch);
			}

			void set_epsilon(node_type* epsilon)
			{
				set_epsilon_impl(epsilon);
			}

			node_type* get_epsilon() const
			{
				return get_epsilon_impl();
			}

			bool is_accept(char_type ch) const
			{
				return is_accept_impl(ch);
			}
		};

		template <typename char_t>
		class nfa_node : public node<char_t>
		{
		public:
			typedef node<char_t> base_type;
			typedef typename base_type::char_type char_type;
			typedef typename base_type::acceptor_type acceptor_type;
			typedef base_type node_type;

		private:
			typedef std::vector<std::pair<acceptor_type*, node_type*> >
			transition_map_type;

			transition_map_type transition_map;
			node_type* epsilon;

			struct eval_is_acceptor
			{
				char_type value;
				eval_is_acceptor(char_type value_):
					value(value_)
				{}

				bool operator()(const std::pair<acceptor_type*,
								node_type*>& elem) const
				{
					return (*elem.first)(value);
				}
			};

			typename transition_map_type::const_iterator
			find_matched_entry(char_type ch) const
			{
				return std::find_if(transition_map.begin(),
									transition_map.end(),
									eval_is_acceptor(ch));
			}

		protected:
			virtual void add_entry_impl(acceptor_type* acceptor,
										node_type* node)
			{
				transition_map.push_back(std::make_pair(acceptor, node));
			}

			virtual node_type* get_transition_impl(char_type ch) const
			{
				typename transition_map_type::const_iterator itor =
					find_matched_entry(ch);

				assert (itor != transition_map.end());

				return itor->second;
			}

			virtual bool is_accept_impl(char_type ch) const
			{
				typename transition_map_type::const_iterator itor =
					find_matched_entry(ch);

				return itor != transition_map.end();
			}

			virtual node_type* get_epsilon_impl() const
			{
				return epsilon;
			}

			virtual void set_epsilon_impl(node_type* new_epsilon)
			{
				epsilon = new_epsilon;
			}

		public:
			nfa_node():
				base_type(), epsilon()
			{}

			nfa_node(node_type* epsilon_):
				base_type(), epsilon(epsilon_)
			{}

			nfa_node(const nfa_node& src):
				base_type(src), epsilon(src.epsilon)
			{}

			~nfa_node()
			{}

		};

		template <typename char_t>
		class dfa_node : public node<char_t>
		{
		public:
			typedef node<char_t> base_type;
			typedef typename base_type::char_type char_type;
			typedef typename base_type::acceptor_type acceptor_type;
			typedef base_type node_type;

		private:
			typedef std::vector<std::pair<acceptor_type*, node_type*> >
			transition_map_type;

			transition_map_type transition_map;

			struct eval_is_acceptor
			{
				char_type value;
				eval_is_acceptor(char_type value_):
					value(value_)
				{}

				bool operator()(const std::pair<acceptor_type*,
								node_type*>& elem) const
				{
					return (*elem.first)(value);
				}
			};

			typename transition_map_type::const_iterator
			find_matched_entry(char_type ch) const
			{
				return std::find_if(transition_map.begin(),
									transition_map.end(),
									eval_is_acceptor(ch));
			}

		protected:
			virtual void add_entry_impl(acceptor_type* acceptor,
										node_type* node)
			{
				transition_map.push_back(std::make_pair(acceptor, node));
			}

			virtual node_type* get_transition_impl(char_type ch) const
			{
				typename transition_map_type::const_iterator itor =
					find_matched_entry(ch);

				assert (itor != transition_map.end());

				return itor->second;
			}

			virtual bool is_accept_impl(char_type ch) const
			{
				typename transition_map_type::const_iterator itor =
					find_matched_entry(ch);

				return itor != transition_map.end();
			}

			virtual node_type* get_epsilon_impl() const
			{
				return NULL;
			}

			virtual void set_epsilon_impl(node_type* /*new_epsilon*/)
			{
				assert (!"can not use this function: "
						"::text::regex::dfa_node<char_t>::set_eplsilon()");

				throw std::runtime_error("can not use this function: "
										 "::text::regex::dfa_node<char_t>::"
										 "set_eplsilon()");
			}

		public:
			dfa_node():
				base_type()
			{}

			dfa_node(const dfa_node& src):
				base_type(src)
			{}
		};

		template <typename char_t>
		class node_manager
		{
		public:
			typedef char_t char_type;
			typedef node<char_type> node_type;
			
		private:
			typedef std::vector<node_type*> node_collection_type;

			node_collection_type nodes;

			struct remover
			{
				template <typename arg_type>
				void operator()(const arg_type* element) const
				{
					delete element;
				}
			};

			node_manager(const node_manager&);
			node_manager& operator=(const node_manager&);

		public:
			node_manager():
				nodes()
			{}

			~node_manager()
			{
				std::for_each(nodes.begin(),
							  nodes.end(),
							  remover());
			}

			template <template <typename> class node_subtype>
			node_type* create_node()
			{
				node_type* new_node = new node_subtype<char_type>();
				nodes.push_back(new_node);

				return new_node;
			}
		};

		template <typename char_t>
		class pattern
		{
		public:
			typedef char_t char_type;
			typedef node<char_type> node_type;
			typedef node_manager<char_type> manager_type;

		private:
			node_type* root;
			manager_type* manager;

		public:
			pattern(node_type* root_, manager_type* manager_):
				root(root_), manager(manager_)
			{}

			~pattern()
			{
				delete manager;
			}
		};


		template <typename char_t>
		class compiler
		{
		public:
			typedef char_t char_type;
			typedef std::basic_string<char_type> string_type;
			typedef node<char_type> node_type;
			typedef pattern<char_type> pattern_type;

		private:
			typedef nfa_node<char_type> nfa_type;
			typedef node_manager<char_type> manager_type;

			typedef typename string_type::const_iterator iterator_type;

			node_type*
			compile_set(iterator_type& current,
						iterator_type& tail,
						manager_type& manager) const
			{
				
			}

			node_type*
			compile_internal(iterator_type& current,
							 iterator_type& tail,
							 manager_type& manager) const
			{
				if (current == tail)
					return NULL;

				char_type ch = *current++;
				node_type* result = NULL;


				switch (ch)
				{
				case '[':
					result = compile_set(current, tail, manager);
				}
			}

		public:
			compiler()
			{}

			pattern_type compile(const string_type& pattern_string) const
			{
				manager_type* manager = new manager_type();
				node_type* root =
					compile_internal(pattern_string, *manager);

				return pattern_type(root, manager);
			}
				
		};
	}
}


#endif /* CPP_LIB_TEXT_REGEX_REGULAR_EXPRESSIONS_HPP_ */
