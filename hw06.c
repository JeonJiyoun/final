#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>

typedef struct Node* NodePtr;
typedef struct RBtree* RBPtr;
int nb = 0;
int insert = 0;
int miss = 0;
int delete = 0;
struct Node {
	int key;
	NodePtr left;
	NodePtr right;
	NodePtr p;

	enum { RED, BLACK } color;
};
NodePtr node_alloc(int newval) {
	NodePtr self = (NodePtr)malloc(sizeof(struct Node));
	self->key = newval;
	self->left = NULL;
	self->right = NULL;
	self->p = NULL;

	return self;
}

struct RBtree {
	NodePtr root;
	NodePtr nil;
};

RBPtr create_nilnode() {
	RBPtr self = (RBPtr)malloc(sizeof(struct RBtree));
	self->nil = (NodePtr)malloc(sizeof(struct Node));
	self->nil->color = BLACK;
	self->root = self->nil;
	return self;
}
void rbt_print(RBPtr self, NodePtr tree, int level) {
	if (tree->right != self->nil)
		rbt_print(self, tree->right, level + 1);
	for (int i = 0; i < level; i++)
		printf("    ");
	printf("%d", tree->key);
	if (tree->color == RED)
		printf("[R]\n");
	else
		printf("[B]\n");
	if (tree->left != self->nil)
		rbt_print(self, tree->left, level + 1);
}
void left_rotate(RBPtr r, NodePtr x) {
	NodePtr temp;
	temp = x->right;
	x->right = temp->left;

	if (temp->left != r->nil) {
		temp->left->p = x;

	}
	temp->p = x->p;
	if (x->p == r->nil) {
		r->root = temp;
	}
	else if (x == x->p->left) {
		x->p->left = temp;
	}
	else {
		x->p->right = temp;
	}
	temp->left = x;
	x->p = temp;


}

void right_rotate(RBPtr r, NodePtr y) {
	NodePtr temp;
	temp = y->left;
	y->left = temp->right;
	if (temp->right != r->nil) {
		temp->right->p = y;
	}
	temp->p = y->p;

	if (y->p == r->nil) {
		r->root = temp;
	}
	else if (y == y->p->left) {
		y->p->left = temp;
	}
	else {
		y->p->right = temp;
	}
	temp->right = y;
	y->p = temp;

}
void insert_fixup(RBPtr r, NodePtr x) {
	NodePtr u = NULL;

	while (x->p->color == RED) {
		if (x->p == (x->p->p)->left) {
			u = x->p->p->right;

			if (u->color == RED) {
				x->p->color = BLACK;
				u->color = BLACK;

				x->p->p->color = RED;

				x = x->p->p;
			
			}
			else {
				if (x == x->p->right) {
					x = x->p;
					left_rotate(r, x);
					

				}
				x->p->color = BLACK;
				x->p->p->color = RED;

				right_rotate(r, x->p->p);
				

			}
		}
		else {
			u = x->p->p->left;

			if (u->color == RED) {
				x->p->color = BLACK;
				u->color = BLACK;

				x->p->p->color = RED;

				x = x->p->p;
				
			}
			else {
				if (x == (x->p)->left) {
					x = x->p;
					right_rotate(r, x);
					
				}
				x->p->color = BLACK;
				x->p->p->color = RED;

				left_rotate(r, x->p->p);
				

			}
		}
	}
	r->root->color = BLACK;
}
void insert_node(RBPtr r, NodePtr tree, NodePtr n) {
	NodePtr p = r->nil;
	n->left = r->nil;
	n->right = r->nil;
	n->color = RED;


	while (tree != r->nil) {
		p = tree;
		if (n->key > tree->key) {
			tree = tree->right;
		}
		else {
			tree = tree->left;
		}
	}
	n->p = p;

	if (p == r->nil) {
		r->root = n;
	}
	else if (n->key > p->key) {
		p->right = n;
	}
	else {
		p->left = n;
	}

	insert_fixup(r, n);
}

void tree_transplant(RBPtr r, NodePtr t, NodePtr c) {
	if (t->p == r->nil) {
		r->root = c;

	}
	else if (t == t->p->left) {
		t->p->left = c;
	}
	else {
		t->p->right = c;
	}
	c->p = t->p;
}

void delete_fixup(RBPtr r, NodePtr x) {
	NodePtr s = NULL;

	while ((x != r->root) && (x->color == BLACK)) {
		if (x == x->p->left) {
			s = x->p->right;

			if (s->color == RED) {
				s->color = BLACK;
				x->p->color = RED;
				left_rotate(r, x->p);
				s = x->p->right;
			}

			if (s->left->color == BLACK&&s->right->color == BLACK) {
				s->color = RED;
				x = x->p;
			}
			else if (s->left->color == RED&&s->right->color == BLACK) {
				s->color = RED;
				s->left->color = BLACK;
				right_rotate(r, s);
				s = s->p->right;
			}
			if (s->right->color == RED) {
				s->color = x->p->color;
				s->right->color = BLACK;
				x->p->color = BLACK;
				left_rotate(r, x->p);

				x = r->root;
			}
		}
		else {
			s = x->p->left;

			if (s->color == RED) {
				s->color = BLACK;
				x->p->color = RED;
				right_rotate(r, x->p);

				s = x->p->left;
			}
			if (s->left->color == BLACK&&s->right->color == BLACK) {
				s->color = RED;
				x = x->p;
			}
			else if (s->right->color == RED&&s->left->color == BLACK) {
				s->color = RED;
				s->right->color = BLACK;
				left_rotate(r, s);
				s = x->p->left;
			}
			if (s->left->color == RED) {
				s->color = x->p->color;
				s->left->color = BLACK;
				x->p->color = BLACK;
				right_rotate(r, x->p);

				x = r->root;
			}
		}
	}
	x->color = BLACK;
}
void delete_node(RBPtr r, int key) {
	NodePtr target = r->root;
	NodePtr temp = NULL;
	NodePtr x = NULL;
	int t_clr;

	while (key != target->key) {
		if (target->key > key) {
			target = target->left;

		}
		else {
			target = target->right;
		}
	}
	t_clr = target->color;

	if (target->left == r->nil) {
		x = target->right;
		tree_transplant(r, target, target->right);
	}
	else if (target->right == r->nil) {
		x = target->left;
		tree_transplant(r, target, target->left);
	}
	else {
		temp = target->right;

		while (temp->left != r->nil) {
			temp = temp->left;
		}
		t_clr = temp->color;
		x = temp->right;

		tree_transplant(r, temp, temp->right);
		temp->right = target->right;
		temp->right->p = temp;

		tree_transplant(r, target, temp);
		temp->left = target->left;
		temp->left->p = temp;
		temp->color = target->color;
	}
	if (t_clr == 1) {
		delete_fixup(r, x);
	}
	free(target);

}

NodePtr search_node(RBPtr r, NodePtr tree, int value) {
	if (tree == r->nil) {
		return tree;
	}
	while (tree->key != value) {
		if (tree->key > value) {
			if (tree->left != r->nil)
				tree = tree->left;
			else {
				return tree;
			}
		}
		else {
			if (tree->right != r->nil)
				tree = tree->right;
			else
				return tree;
		}
	}

	return tree;
}
void rbt_inorder(RBPtr self, NodePtr tree) {
	if (tree == self->nil)
		return;
	else {
		rbt_inorder(self, tree->left);
		printf("%d ", tree->key);
		if (tree->color == RED) {
			printf("R\n");
		}
		else
			printf("B\n");
		rbt_inorder(self, tree->right);
	}

}
void numofnb(RBPtr self, NodePtr tree) {
	int i = 1;
	if (tree == self->nil)
		return;
	else {
		numofnb(self, tree->left);
		numofnb(self, tree->right);
	}
	if (tree->color == BLACK) {
		nb++;
	}
}


int BlackHeight(RBPtr self, NodePtr root) {
	int i = 0;
	while (root != self->nil) {
		if (root->color == BLACK) {
			i++;
		}
		root = root->left;
	}
	return i;
}
NodePtr minimum(RBPtr self, NodePtr tree) {
	while (tree->left != self->nil) {
		tree = tree->left;
	}
	return tree;
}
NodePtr maximum(RBPtr self, NodePtr tree) {
	while (tree->right != self->nil) {
		tree = tree->right;
	}
	return tree;
}
int succesor(RBPtr self, NodePtr tree) {
	NodePtr y = NULL;
	if (tree->right != self->nil) {
		return minimum(self, tree->right)->key;
	}
	y = tree->p;
	while (y != self->nil&&tree == y->right) {
		tree = y;
		y = y->p;
	}
	return y->key;
}
int predecessor(RBPtr self, NodePtr tree) {
	NodePtr y = NULL;
	if (tree->left != self->nil) {
		return maximum(self, tree->left)->key;
	}
	y = tree->p;
	while (y != self->nil&&tree == y->left) {
		tree = y;
		y = y->p;
	}
	return y->key;
}
int main() {

	RBPtr rbt = create_nilnode();
	FILE *fp;
	FILE *out;
	NodePtr x = NULL;
	int data, s;
	int not_exist = 0;
	int total = 0;
	fp = fopen("test01.txt", "r");
	while (fscanf(fp, "%d", &data) != EOF) {

		if (data > 0) {
			int z = data;
			insert_node(rbt, rbt->root, node_alloc(data));
			total++;
			insert++;
		}
		else if (data<0) {
			int y = -data;
			NodePtr x = search_node(rbt, rbt->root, y);
			if (x->key != y) {
				not_exist++;
			}
			else {
				delete_node(rbt, x->key);
				total--;
				delete++;

			}
		}
		else {
			break;

		}
	}
	fclose(fp);
	out = fopen("output01.txt", "wt");
	fp = fopen("search01.txt", "r");
	while (fscanf(fp, "%d", &data) != EOF) {
		s = data;
		if (data == 0) {
			break;
		}
		if (search_node(rbt, rbt->root, s)->key == data) {
			x = search_node(rbt, rbt->root, s);
			if (x == minimum(rbt, rbt->root) && x == maximum(rbt, rbt->root)) {
				fprintf(out, "NIL %d NIL\n", x->key);
			}
			else if (x == minimum(rbt, rbt->root)) {
				int su = succesor(rbt, x);
				fprintf(out, "NIL %d %d \n", x->key, su);
			}
			else if (x == maximum(rbt, rbt->root)) {
				int pre = predecessor(rbt, x);
				fprintf(out, "%d %d NIL\n", pre, x->key);
			}
			else {
				int su = succesor(rbt, x);
				int pre = predecessor(rbt, x);
				fprintf(out, "%d %d %d\n", pre, x->key, su);
			}
		}
		else {
			int i = s;
			while(1) {
				if (search_node(rbt, rbt->root, i)->key == i) {
					fprintf(out, "%d ", i);
					break;
				}
				if(i==0) {
					fprintf(out, "NIL ");
					break;
				}
				
				i--;
			}
			fprintf(out, "NIL ");
			i = s;
			while(1) {
				
				if (search_node(rbt, rbt->root, i)->key == i) {
					fprintf(out, "%d \n", i);
					break;
				}
				if(i>maximum(rbt,rbt->root)->key){
					fprintf(out,"NIL\n");
					break;
				}
				i++;
			}
		}
		if (data == 0)
			break;
	}
	fclose(fp);
	fclose(out);
	printf("total = %d\n", total);
	printf("insert = %d\n",insert);
	printf("deleted = %d\n", delete);
	printf("miss = %d\n", not_exist);
	numofnb(rbt, rbt->root);
	printf("nb = %d\n", nb);
	printf("bh = %d\n\n", BlackHeight(rbt, rbt->root));
	rbt_inorder(rbt, rbt->root);
	printf("\n");
	rbt_print(rbt,rbt->root,0);
	
	return 0;
}
