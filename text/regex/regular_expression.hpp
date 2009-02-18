#ifndef CPP_LIB_TEXT_REGEX_REGULAR_EXPRESSIONS_HPP_
#define CPP_LIB_TEXT_REGEX_REGULAR_EXPRESSIONS_HPP_

#include <map>
#include <string>
#include <vector>
#include <algorithm>

namespace text 
{
	namespace regex
	{
		template <typename char_t>
		struct node
		{
			typedef char_t char_type;

			typedef typename std::map<char_type, node*> transition_map_type;

			transition_map_type transition_map;

			node():
				transition_map()
			{}

			node(const node& src):
				transition_map(src.transition_map)
			{}

			virtual ~node()
			{}

			void add_entry(char_type ch, node* node)
			{
				transition_map[ch] = node;
			}

			void remove_entry(char_type ch)
			{
				typename transition_map_type::iterator entry =
					transition_map.find(ch);
				if (entry != transition_map.end())
					transition_map.erase(entry);
			}

			node* get_entry(char_type ch) const
			{
				typename transition_map_type::const_iterator itor =
					transition_map.find(ch);

				if (itor != transition_map.end())
					return itor->second;

				return NULL;
			}

			bool is_accept(char_type ch) const
			{
				return transition_map.find(ch) != transition_map.end();
			}
		};

		template <typename char_t>
		struct nfa_node : public node<char_t>
		{
			typedef node<char_t> base_type;
			typedef typename base_type::char_type char_type;

			base_type* epsilon;

			nfa_node():
				base_type(), epsilon()
			{}

			nfa_node(base_type* epsilon_):
				base_type(), epsilon(epsilon_)
			{}

			nfa_node(const nfa_node& src):
				base_type(src), epsilon(src.epsilon)
			{}

			~nfa_node()
			{}

			base_type* get_epsilon() const
			{
				return epsilon;
			}

			void set_epsilon(base_type* new_epsilon)
			{
				epsilon = new_epsilon;
			}
		};

		template <typename char_t>
		struct dfa_node : public node<char_t>
		{
			typedef node<char_t> base_type;
			typedef typename base_type::char_type char_type;

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
