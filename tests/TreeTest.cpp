#include <cppunit/extensions/HelperMacros.h>
#include <util/Tree.hpp>

class TreeTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(TreeTest);
	CPPUNIT_TEST(traverseTest);
	CPPUNIT_TEST_SUITE_END();

private:
	void traverseTest()
	{
		Tree<Node> tree;
		Node* p1;
		Node* p2;
		Node* p3;

		/**
		 *                r
		 *            / /   \ \
		 *           n n(p1) n n
		 *      /////  /       \
		 *  nnn(p2)nn  n        n(p3)
		 */
		tree.getRoot()->addChild(new Node());
		tree.getRoot()->addChild(p1 = new Node());
		tree.getRoot()->addChild(new Node());
		tree.getRoot()->addChild(new Node());
		std::vector<Node*> level1nodes = tree.getRoot()->getChildren();
		level1nodes[0]->addChild(new Node());
		level1nodes[0]->addChild(new Node());
		level1nodes[0]->addChild(p2 = new Node());
		level1nodes[0]->addChild(new Node());
		level1nodes[0]->addChild(new Node());

		level1nodes[1]->addChild(new Node());
		
		level1nodes[3]->addChild(p3 = new Node());


	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( TreeTest );

