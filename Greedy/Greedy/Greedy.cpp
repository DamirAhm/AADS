#include <iostream> 
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <ctime>


using namespace std;

struct point {
	double x, y;
};

point resultA, resultB;

int read_dots(ifstream& file, point* a) {
	int dot_index = 0;
	while (!file.eof()) {
		file >> a[dot_index].x;
		file >> a[dot_index].y;

		dot_index++;
	}

	return dot_index;
}

bool x_predicate(const point& a, const point& b) {
	return a.x < b.x || a.x == b.x && a.y < b.y;
}
bool y_predicate(const point& a, const point& b) {
	return a.y < b.y || a.y == b.y && a.x < b.x;
}

double dots_dist(const point& a, const point& b) {
	return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

void algorithm(point* a, double& min_dist, int last, int first = 0);
void algorithm(point* a, double& min_dist, int last, int first) {
	if (last - first <= 3) {
		for (int i = first; i <= last; ++i) {
			for (int j = i + 1; j <= last; ++j) {
				double dist = dots_dist(a[i], a[j]);

				if (dist < min_dist) {
					min_dist = dist;
					resultA = a[i];
					resultB = a[j];
				}
			}
		}

		sort(a + first, a + last + 1, &y_predicate);
		return;
	}

	int mid = (first + last) / 2;
	int mid_x = a[mid].x;

	algorithm(a, min_dist, mid, first);
	algorithm(a, min_dist, last, mid + 1);

	static point b[20000];

	merge(a + first, a + mid + 1, a + mid + 1, a + last + 1, b, &y_predicate);
	copy(b, b + last - first + 1, a + first);

	int tsz = 0;
	for (int i = first; i <= last; ++i) {
		if (abs(a[i].x - mid_x) < min_dist) {
			for (int j = tsz - 1; j >= 0 && a[i].y - b[j].y < min_dist; --j) {
				double dist = dots_dist(a[i], b[j]);

				if (dist < min_dist) {
					min_dist = dist;
					resultA = a[i];
					resultB = b[j];
				}
			}

			b[tsz++] = a[i];
		}
	}
}

int main() {
	int startTime = clock();
	double min_dist = DBL_MAX;
	point dots[20000];

	ifstream file("input.txt");

	int dots_amount = read_dots(file, dots);

	sort(dots, dots + dots_amount, &x_predicate);
	algorithm(dots, min_dist, dots_amount - 1);

	cout << resultA.x << " " << resultA.y << " | " << resultB.x << " " << resultB.y << endl;

	cout << "Minimal distance between two points is: " << dots_dist(resultA, resultB) << endl;
	int endTime = clock();

	cout << "Time: " << endTime - startTime << endl;

	return 0;
}