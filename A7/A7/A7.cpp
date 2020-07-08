#include <stdio.h>
#include <limits.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

int N;															//���������� ������������� ������
vector<int> m_sizes; 											//������� ������ (1*2, 2*3 ...)
vector<vector<vector<int>>> matrixs;							//�������
vector<vector<int>> m_matrix;									//������� ���������� ������������
vector<vector<int>> m_result;									//���������
vector<vector<pair<int,int>>> s_matrix;							//������� �������� ������
vector<vector<pair<pair<int, int>, pair<int, int>>>> n_matrix;	//������� ��������� ������

/*������� �������������� ��������� ���������, �������� ������ � �������� ������� ���������*/
void MatrixChainOrder() {
	//��������� �������
	vector<vector<int>> m_mat(N, vector<int>(N));
	vector<vector<pair<int, int>>> s_mat(N, vector<pair<int, int>>(N));
	vector<vector<pair<pair<int, int>, pair<int, int>>>> n_mat(N, vector<pair<pair<int, int>, pair<int, int>>>(N));

	//������������� ��������������� ������
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

	// L - ����� ������� ��������
	for (int L = 2; L <= N; L++) {
		//�������� ���������, ���� ������� ���������
		for (int i = 0; i < N - L + 1; i++) {
			int j = i + L - 1;
			m_mat[i][j] = INT_MAX; //������ ������������� ��������

			for (int k = i; k <= j - 1; k++) {
				//cost - ��������� ������������/����� ��������
				//k - ������ � ������, ��� ������ ����������(���� �.�.)
				//��� ����� ����� ���-�� ��������, ����������� �� ������� �������� + ��������� ��������� ������
				//�������� ��������� ������� �� ���� ���� = 0
				int cost = m_mat[i][k] + m_mat[k + 1][j] + (m_sizes[i] * m_sizes[k + 1] * m_sizes[j + 1]); 

				//���� �������� ������ ������������ � ������, �� ����� ��������
				if (cost < m_mat[i][j]) {
					m_mat[i][j] = cost;	//������ ���������
					n_mat[i][j] = { { i,k },{ k + 1,j } }; //������ ���������
					s_mat[i][j] = { m_sizes[i],m_sizes[j + 1] }; //������ ������� �������
				}
			}
		}
	}

	//����������� �������
	m_matrix = m_mat;
	s_matrix = s_mat;
	n_matrix = n_mat;
}

/*������� ��������� 2-�� ������*/
vector<vector<int>> matrixMultiply(vector<vector<int>> A, vector<vector<int>> B) {
	int X = A.size();		//���������� ����� � �
	int Y = B[0].size();	//���������� �������� �
	int L = B.size();		//���������� ����� � �

	//��������� ������ X*Y
	vector<vector<int>> result(X, vector<int>(Y));

	//��������� ��������� 2-�� ������
	for (int i = 0; i < X; i++)
		for (int j = 0; j < Y; j++)
			for (int k = 0; k < L; k++)
				result[i][j] += A[i][k] * B[k][j];

	return result;
}


/*������� ��������� i ���������� ������*/
vector<vector<int>> MatrixMultiplyOrder(int x, int y) {

	vector<vector<int>> result; //�������� ���������

	//��������� ��������� ������
	if ((n_matrix[x][y].first.first == n_matrix[x][y].first.second) && (n_matrix[x][y].second.first == n_matrix[x][y].second.second)) {
		vector<vector<int>> A = matrixs[n_matrix[x][y].first.first];
		vector<vector<int>> B = matrixs[n_matrix[x][y].second.first];
		result = matrixMultiply(A, B);
		return result;
	}
	else
		//��������� �� ��������� ������
		if (n_matrix[x][y].first.first != n_matrix[x][y].first.second && n_matrix[x][y].second.first != n_matrix[x][y].second.second) {
			result = matrixMultiply(MatrixMultiplyOrder(n_matrix[x][y].first.first, n_matrix[x][y].first.second),
				MatrixMultiplyOrder(n_matrix[x][y].second.first, n_matrix[x][y].second.second));
			return result;
		}
		else
			//��������� ����� ��������� � ���� ��� ������(������ 1-�����������)
			if (n_matrix[x][y].first.first != n_matrix[x][y].first.second) {;
				vector<vector<int>> B = matrixs[n_matrix[x][y].second.first];		
				result = matrixMultiply(MatrixMultiplyOrder(n_matrix[x][y].first.first, n_matrix[x][y].first.second) , B);
				return result;
			}
			else
				//��������� ����� ��������� � ���� ��� ������(������ 2-�����������)
				if (n_matrix[x][y].second.first != n_matrix[x][y].second.second) {
					vector<vector<int>> A = matrixs[n_matrix[x][y].first.first];
					result = matrixMultiply(A, MatrixMultiplyOrder(n_matrix[x][y].second.first, n_matrix[x][y].second.second));
					return result;
				}

	return result;			
}

/*���������� ������ � ������������� ��������*/
string GenerateString(int x, int y) {
	string str = ""; //�������� ������ � �������������� �������� � ���������

	//������������ ������ ��������� ������
	if ((n_matrix[x][y].first.first == n_matrix[x][y].first.second) && (n_matrix[x][y].second.first == n_matrix[x][y].second.second)) {	
		str += "(A"; str += to_string(n_matrix[x][y].first.first + 1);
		str += "*A"; str += to_string(n_matrix[x][y].second.first + 1);
		str += ")";
		return str;
	}
	else
		//������������ ������ �� ��������� ������
		if (n_matrix[x][y].first.first != n_matrix[x][y].first.second && n_matrix[x][y].second.first != n_matrix[x][y].second.second) {
			str += "("
				+ GenerateString(n_matrix[x][y].first.first, n_matrix[x][y].first.second)
				+ "*"
				+ GenerateString(n_matrix[x][y].second.first, n_matrix[x][y].second.second)
				+ ")";
		}
		else
			//������������ ������ ����� ��������� � ���� ��� ������(������ 1-�����������)
			if (n_matrix[x][y].first.first != n_matrix[x][y].first.second ) {
				str += "(";
				str += GenerateString(n_matrix[x][y].first.first, n_matrix[x][y].first.second);
				str += "*A" + to_string(n_matrix[x][y].second.first + 1);
				str += ")";
			}
			else
				//������������ ������ ����� ��������� � ���� ��� ������(������ 2-�����������)
				if (n_matrix[x][y].second.first != n_matrix[x][y].second.second) {
					str += "(A";
					str += to_string(n_matrix[x][y].first.first + 1);
					str += "*";
					str += GenerateString(n_matrix[x][y].second.first, n_matrix[x][y].second.second);
					str += ")";
				}
	return str;
}

/*�������� �������*/
int main() {
	setlocale(LC_ALL, "rus");
	//������
	ifstream ifs;
	ofstream ofs;

	//������ �� �����
	ifs.open("input.txt");
	ifs >> N;
	for (int i = 0; i < N + 1; i++) {
		int tmp; ifs >> tmp;
		m_sizes.push_back(tmp);
	}

	//���������� ������
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

	//���������� ������� ��������������
	MatrixChainOrder();

	//����� � ����
	ofs.open("output.txt");

	//����� ��������� ������
	ofs << "�������� �������: " << endl;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < m_sizes[i]; j++) {
			for (int k = 0; k < m_sizes[i + 1]; k++) {
				ofs << "  " << matrixs[i][j][k];
			}
			ofs << endl;
		}
		ofs << endl;
	}

	//����� �������� �� ��������� �������� �������
	ofs << "���������� ����� ��������, ����������� ��� ������������ ������: " << m_matrix[0][N - 1] << endl;
	ofs << "����������� �����������" << GenerateString(0, N - 1) << endl;

	//����� ���������� ������������
	ofs << endl << "��������� ������������ ������: " << endl;
	m_result = MatrixMultiplyOrder(0, N - 1);
	for (int i = 0; i < m_result.size(); i++) {
		for (int j = 0; j < m_result[0].size(); j++)
			ofs << "  " << m_result[i][j];
		ofs << endl;
	}

	//����� ������� ���������� ������������
	ofs << endl << "������� ����� �������� ������������: " << endl;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++)
			ofs << "  " <<  m_matrix[i][j];
		ofs << endl;
	}

	//����� ������� ��������
	ofs << endl << "������� �������� ������: " << endl;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++)
			ofs << "  " << s_matrix[i][j].first << "*" << s_matrix[i][j].second;
		ofs << endl;
	}

	ofs << endl << "������� ������������ ���������� ������/������������: " << endl;
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