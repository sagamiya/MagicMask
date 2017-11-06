

class MaskRect {
public:
	MaskRect * next;
	MaskRect * prev;
	int x,y,w,h;

	MaskRect(int x, int y, int w, int h) {
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
		next = NULL;
		prev = NULL;
	}

	void ExpandRight(int delta) {
		w += delta;
	}
	void ExpandLeft(int delta) {
		x -= delta;
		w += delta;
	}
	void ExpandUp(int delta) {
		y -= delta;
		h += delta;
	}
	void ExpandDown(int delta) {
		h += delta;
	}
};

class Mask {
public:
	MaskRect * root;
	int		width;
	int		height;
public:
	Mask(int w, int h) {
		width = w;
		height = h;
		root = new MaskRect(0, 0, 0, 0);
		MaskRect * initRect = new MaskRect(0, 0, w, h);
		initRect->prev = root;
		root->next = initRect;
	}
	~Mask() {
		while (root!=NULL) {
			MaskRect *tmp = root->next;
			delete root;
			root = tmp;
		}
	}
	void openWindow (int nHorPos, int nVerPos, int nWidth, int nHeight);
	void shakeout(int limit);
private:
	void remove(MaskRect *node);
	void add(MaskRect *node);
};