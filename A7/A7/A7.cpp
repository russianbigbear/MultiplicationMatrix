#include <stdio.h>
#include <limits.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

int N;															//количество перемножаемых матриц
vector<int> m_sizes; 											//размеры матриц (1*2, 2*3 ...)
vector<vector<vector<int>>> matrixs;							//матрицы
vector<vector<int>> m_matrix;									//матрица количества перемножений
vector<vector<int>> m_result;									//результат
vector<vector<pair<int,int>>> s_matrix;							//матрица размеров матриц
vector<vector<pair<pair<int, int>, pair<int, int>>>> n_matrix;	//матрица умножения матриц

/*Функция предвычисления стоимости умножения, размеров матриц и создании матрицы умножений*/
void MatrixChainOrder() {
	//формируем матрицы
	vector<vector<int>> m_mat(N, vector<int>(N));
	vector<vector<pair<int, int>>> s_mat(N, vector<pair<int, int>>(N));
	vector<vector<pair<pair<int, int>, pair<int, int>>>> n_mat(N, vector<pair<pair<int, int>, pair<int, int>>>(N));

	//инициализация вспомогательных матриц
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++) {
			m_mat[i][j] = 0;
			n_mat[i][j] = { { 0,0 },{ 0,0 } };

			if (i != j) {
				s_mat[i][j] = { 0,0 };
			}
			else {
				s_mat[i][j] = { m_sizes[i],m_sizes[i + 1] };
			}
		}

	// L - длина цепочки размеров
	for (int L = 2; L <= N; L++) {
		//значения стоимости, выше главной диагонали
		for (int i = 0; i < N - L + 1; i++) {
			int j = i + L - 1;
			m_mat[i][j] = INT_MAX; //запись максимального значения

			for (int k = i; k <= j - 1; k++) {
				//cost - стоимость перемножения/число операций
				//k - ячейка в строке, для записи результата(выше г.д.)
				//она будет равно кол-ву операций, потраченное на решение подзадач + стоимость умножения матриц
				//стомость умножения матрицы на саму себя = 0
				int cost = m_mat[i][k] + m_mat[k + 1][j] + (m_sizes[i] * m_sizes[k + 1] * m_sizes[j + 1]); 

				//если значение меньше максимальной в ячейке, то пишим значение
				if (cost < m_mat[i][j]) {
					m_mat[i][j] = cost;	//запись стоимости
					n_mat[i][j] = { { i,k },{ k + 1,j } }; //запись умножения
					s_mat[i][j] = { m_sizes[i],m_sizes[j + 1] }; //запись размера матрицы
				}
			}
		}
	}

	//присваиваем матрицы
	m_matrix = m_mat;
	s_matrix = s_mat;
	n_matrix = n_mat;
}

/*Фукнция умножения 2-ух матриц*/
vector<vector<int>> matrixMultiply(vector<vector<int>> A, vector<vector<int>> B) {
	int X = A.size();		//количество строк в А
	int Y = B[0].size();	//количество столбцов В
	int L = B.size();		//количествр строк в В

	//формируем вектор X*Y
	vector<vector<int>> result(X, vector<int>(Y));

	//вычисляем умножение 2-ух матриц
	for (int i = 0; i < X; i++)
		for (int j = 0; j < Y; j++)
			for (int k = 0; k < L; k++)
				result[i][j] += A[i][k] * B[k][j];

	return result;
}


/*Функция умножения i количества матриц*/
vector<vector<int>> MatrixMultiplyOrder(int x, int y) {

	vector<vector<int>> result; //конечный результат

	//умножение начальных матриц
	if ((n_matrix[x][y].first.first == n_matrix[x][y].first.second) && (n_matrix[x][y].second.first == n_matrix[x][y].second.second)) {
		vector<vector<int>> A = matrixs[n_matrix[x][y].first.first];
		vector<vector<int>> B = matrixs[n_matrix[x][y].second.first];
		result = matrixMultiply(A, B);
		return result;
	}
	else
		//умножение не начальных матриц
		if (n_matrix[x][y].first.first != n_matrix[x][y].first.second && n_matrix[x][y].second.first != n_matrix[x][y].second.second) {
			result = matrixMultiply(MatrixMultiplyOrder(n_matrix[x][y].first.first, n_matrix[x][y].first.second),
				MatrixMultiplyOrder(n_matrix[x][y].second.first, n_matrix[x][y].second.second));
			return result;
		}
		else
			//умножение одной начальной и одно нет матриц(случай 1-неначальная)
			if (n_matrix[x][y].first.first != n_matrix[x][y].first.second) {;
				vector<vector<int>> B = matrixs[n_matrix[x][y].second.first];		
				result = matrixMultiply(MatrixMultiplyOrder(n_matrix[x][y].first.first, n_matrix[x][y].first.second) , B);
				return result;
			}
			else
				//умножение одной начальной и одно нет матриц(случай 2-неначальная)
				if (n_matrix[x][y].second.first != n_matrix[x][y].second.second) {
					vector<vector<int>> A = matrixs[n_matrix[x][y].first.first];
					result = matrixMultiply(A, MatrixMultiplyOrder(n_matrix[x][y].second.first, n_matrix[x][y].second.second));
					return result;
				}

	return result;			
}

/*Построение строки с расставлеными скобками*/
string GenerateString(int x, int y) {
	string str = ""; //конечная строка с расставленными скобками и индексами

	//формирования строки начальных матриц
	if ((n_matrix[x][y].first.first == n_matrix[x][y].first.second) && (n_matrix[x][y].second.first == n_matrix[x][y].second.second)) {	
		str += "(A"; str += to_string(n_matrix[x][y].first.first + 1);
		str += "*A"; str += to_string(n_matrix[x][y].second.first + 1);
		str += ")";
		return str;
	}
	else
		//формирования строки не начальных матриц
		if (n_matrix[x][y].first.first != n_matrix[x][y].first.second && n_matrix[x][y].second.first != n_matrix[x][y].second.second) {
			str += "("
				+ GenerateString(n_matrix[x][y].first.first, n_matrix[x][y].first.second)
				+ "*"
				+ GenerateString(n_matrix[x][y].second.first, n_matrix[x][y].second.second)
				+ ")";
		}
		else
			//формирования строки одной начальной и одно нет матриц(случай 1-неначальная)
			if (n_matrix[x][y].first.first != n_matrix[x][y].first.second ) {
				str += "(";
				str += GenerateString(n_matrix[x][y].first.first, n_matrix[x][y].first.second);
				str += "*A" + to_string(n_matrix[x][y].second.first + 1);
				str += ")";
			}
			else
				//формирования строки одной начальной и одно нет матриц(случай 2-неначальная)
				if (n_matrix[x][y].second.first != n_matrix[x][y].second.second) {
					str += "(A";
					str += to_string(n_matrix[x][y].first.first + 1);
					str += "*";
					str += GenerateString(n_matrix[x][y].second.first, n_matrix[x][y].second.second);
					str += ")";
				}
	return str;
}

/*Основная функция*/
int main() {
	setlocale(LC_ALL, "rus");
	//потоки
	ifstream ifs;
	ofstream ofs;

	//чтение из файла
	ifs.open("input.txt");
	ifs >> N;
	for (int i = 0; i < N + 1; i++) {
		int tmp; ifs >> tmp;
		m_sizes.push_back(tmp);
	}

	//считывание матриц
	for (int i = 0; i < N; i++) {
		matrixs.push_back(vector<vector<int>>());
		for (int j = 0; j < m_sizes[i]; j++) {
			matrixs[i].push_back(vector<int>());
			for (int k = 0; k < m_sizes[i + 1]; k++) {
				int tmp; ifs >> tmp;
				matrixs[i][j].push_back(tmp);
			}
		}
	}
	ifs.close();

	//выполнение функции предвычислений
	MatrixChainOrder();

	//вывод в файл
	ofs.open("output.txt");

	//вывод начальных матриц
	ofs << "Введеные матрицы: " << endl;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < m_sizes[i]; j++) {
			for (int k = 0; k < m_sizes[i + 1]; k++) {
				ofs << "  " << matrixs[i][j][k];
			}
			ofs << endl;
		}
		ofs << endl;
	}

	//вывод сведений об отработке основной функции
	ofs << "Минимально число операций, требующихся для перемножения матриц: " << m_matrix[0][N - 1] << endl;
	ofs << "Оптимальная расстановка" << GenerateString(0, N - 1) << endl;

	//вывод пезультата перемножения
	ofs << endl << "Результат перемножения матриц: " << endl;
	m_result = MatrixMultiplyOrder(0, N - 1);
	for (int i = 0; i < m_result.size(); i++) {
		for (int j = 0; j < m_result[0].size(); j++)
			ofs << "  " << m_result[i][j];
		ofs << endl;
	}

	//вывод матрицы количества перемножений
	ofs << endl << "Матрица числа операций перемножений: " << endl;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++)
			ofs << "  " <<  m_matrix[i][j];
		ofs << endl;
	}

	//вывод матрицы размеров
	ofs << endl << "Матрица размеров матриц: " << endl;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++)
			ofs << "  " << s_matrix[i][j].first << "*" << s_matrix[i][j].second;
		ofs << endl;
	}

	ofs << endl << "Матрица рекурсивного построения строки/перемножения: " << endl;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++)
			ofs << "  "
			<< "(" << n_matrix[i][j].first.first << ","
			<< n_matrix[i][j].first.second << ")*("
			<< n_matrix[i][j].second.first << ","
			<< n_matrix[i][j].second.second << ")";

		ofs << endl;
	}

	ofs.close();

	system("output.txt");
}