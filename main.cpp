#define NOMINMAX        // Важно: отключает макросы min/max из Windows.h
#define WIN32_LEAN_AND_MEAN
#include <windows.h>    // Сначала Windows.h с макросами
#include <iostream>
#include <vector>
#include <iomanip>
#include <sstream>
#include <conio.h>

using namespace std;
//устанавливает цвет
void setColor(int color) {
    HANDLE hColor = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hColor, color);
}

//выводит текст по указанному цвету
void printColor(const string& text, int color) {
    setColor(color);
    cout << text;
}

void clearScreen(int bgAttr = 0x0F) {   // 0x0F = чёрный фон, белый текст
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(h, &csbi);

    DWORD size = csbi.dwSize.X * csbi.dwSize.Y;
    DWORD written;
    COORD home = { 0, 0 };

    // заливаем пробелами с нужным атрибутом (фон + текст)
    FillConsoleOutputCharacterA(h, ' ', size, home, &written);
    FillConsoleOutputAttribute(h, bgAttr, size, home, &written);
    SetConsoleCursorPosition(h, home);
}

//количество строк и столбцов
int getint(const string& expression) {
    double value;
    printColor(expression, 07);

    bool v = true;
    while (v) {
        cin >> value;
        if (cin.fail()) {
            printColor("Ошибка!!! Введите число: ", 0x0C);
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        else if (value != int(value)) {
            printColor("Вы ввели дробное число! Введите целое число: ", 0x0C);
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        else if (value <= 0) {
            printColor("Вы ввели отрицательное число! Введите положительное число: ", 0x0C);
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            v = false;
        }
    }
    return static_cast<int>(value);
}
//для пермещения курсора
void setCursorPosition(SHORT x,SHORT y) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = { x, y }; //создает структуру с координатами
    SetConsoleCursorPosition(hConsole, pos); //перемещает курсор в доступную позицию
}
//получить позицию курсора
COORD getCursorPosition() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); //получает дескриптор консоли
    CONSOLE_SCREEN_BUFFER_INFO c; //создает структуру для информации о буфере экрана
    GetConsoleScreenBufferInfo(hConsole, &c); //заполняет структурами данными о состоянии консоли
    return c.dwCursorPosition; //возвращает координаты куросра
}
string doubleToString(double value) {
    string editBuffer = to_string(value);
    size_t pos = editBuffer.find(',');
    if (pos != string::npos) {
        editBuffer.erase(editBuffer.find_last_not_of('0') + 1, string::npos);
        if (!editBuffer.empty() && editBuffer.back() == ',') {
            editBuffer.pop_back();
        }
    }
    return editBuffer;
}
//рисовка таблицы
void picterMatrix(const vector<vector<double>>& matrix, int currentRow, int currentCol, bool edit = false, const string& editBuffer = "") {
    size_t rows = matrix.size(), cols = matrix[0].size();
    //вверхняя граница
    printColor("|-", 0x0E);
    for (int i = 0; i < cols; i++) {
        printColor("----------", 0x0E);
        if (i < cols - 1) printColor("-|-", 0x0E); //соединение, если не последняя колонка
    }
    printColor("-|\n", 0x0E);

    for (int i = 0; i < rows; i++) {
        printColor("|", 0x0E);
        for (int j = 0; j < cols; j++) {
            if (i == currentRow && j == currentCol) { //активная ячейка в режиме редактирования
                if (edit) { //навигация
                    setColor(0x1F);//красная ячейка
                    cout << setw(10) << left << editBuffer << "_";
  
                    printColor("  |", 0x0E);
                }
                else { //режим ввода
                    setColor(0x1F);
                    cout << setw(10) <<left << doubleToString(matrix[i][j]);
                     printColor("  |", 0x0E);
                }
            }
            else { //обычная ячейка
                setColor(0x0F);
                cout << setw(10) << left << doubleToString(matrix[i][j]);
                printColor("  |", 0x0E);
                
            }
        }
        cout << endl;
        //между строк разделители
        if (i < rows - 1) {
            printColor("|-", 0x0E);
            for (int j = 0; j < cols; j++) {
                printColor("----------", 0x0E);
                if (j < cols - 1) printColor("-|-", 0x0E);
            }
            printColor("-|\n", 0x0E);
        }
    }
    //нижняя граница
    printColor("|_", 0x0E);
    for (int i = 0; i < cols; i++) {
        printColor("__________", 0x0E);
        if (i < cols - 1) printColor("_|_", 0x0E);
    }
    printColor("_|\n", 0x0E);
}
//редактирование значения в ячейке
string editingValue(double Value, int currentRow, int currentCol) {
    string editBuffer = doubleToString(Value);

    bool editing = true;
    while (editing) {
        COORD origPos = getCursorPosition(); //где был курсор до начала орерации
        
        int cellX = 1 + (currentCol * 13); //расчет позиции(отступ + номер колонки * ширина)
        int cellY = 1 + (currentRow * 2);

        setCursorPosition(cellX, cellY); //перемещаем курсор на ячейку
        setColor(0x1F);
        cout << string(10, ' ');
        setCursorPosition(cellX, cellY);
        cout << editBuffer << "_";
        setCursorPosition(cellX + static_cast<int>(editBuffer.length()), cellY);

        int key = _getch(); //функция читает один сивол, возвращает аски код клавиши
        if (key == 13) {//enter
 
            editing = false;}
        else if (key == 27) {//esk отмена редактирования
            editBuffer = doubleToString(Value); //исходное число - строка
            editing = false;
        }
        else if (key == 8) {//baskape удалить символ
            if (!editBuffer.empty()) //проверка на пустоту, если не пусто
                editBuffer.pop_back(); //удаляем последний элемент
        }
        else if (key == 44 || (key >= 48 && key <= 57) || key == 45) {//запятая, цифры, минус
            if (editBuffer.length() < 10)
                editBuffer += static_cast<char>(key);//преобразует целое число в чар и добавляет 
        }

        setCursorPosition(origPos.X, origPos.Y); //возвращаем курсор на место 
    }
    return editBuffer;
}

//создание матрицы
vector<vector<double>> getUserMatrix(const string& matrix_name) {
    clearScreen();
    printColor(matrix_name, 0x0B);
    int row = getint("Введите количество строк: ");
    int col = getint("Введите количество столбцов: ");
    cout.setf(ios::right);
    cout.fill(' ');
    vector<vector<double>> matrix(row, vector<double>(col, 0.0)); 

    int currentRow = 0, currentCol = 0;
    bool inputComp = false;

    while (!inputComp) {
        clearScreen();
        picterMatrix(matrix, currentRow, currentCol);
        printColor("\nУправление", 0x0B);
        printColor("\nСтрелочки - перемещение по ячейкам\n", 0x0D);
        printColor("Enter - редактировать ячейку\\завершить редактирование\n", 0x0D);
        printColor("Ecs - отменить редактирование\\отмена введеного значения\n", 0x0D);
        printColor("Baskspace - удалить символ\n", 0x0D);
        printColor("Цифры, минус, запятая - ввод значения\n", 0x0D);


        int key = _getch();
        if (key == 0 || key == 224) {//проверка это стрелка
            key = _getch();
            switch (key) {
            case 72: //стрелка вверх
                if (currentRow > 0) currentRow--;
                break;
            case 80: //стрелка вниз
                if (currentRow < row - 1) currentRow++;
                break;
            case 75: //стрелка влево
                if (currentCol > 0) currentCol--;
                break;
            case 77:
                if (currentCol < col - 1) currentCol++;
            }
        }
        else {
            switch (key) {
            case 13: {//enter
                string newValue = editingValue(matrix[currentRow][currentCol], currentRow, currentCol);
                if (!newValue.empty()) 
                    matrix[currentRow][currentCol] = stod(newValue);
            }break;

            case 27://esc
                clearScreen();
                picterMatrix(matrix, -1, -1);//показывает матрицу
                printColor("\nЗавершить ввод матрицы?\n", 0x0C);
                printColor("Y - да\n", 0x0E);
                printColor("N - нет\n", 0x0A);
                printColor("\nВаш выбор: \n", 0x0C);

                char answer;
                cin.get(answer);
                if (answer == 'Y' || answer == 'y') {
                    inputComp = true;
                    printColor("Матрица сохранена!", 0x0E);
                    (void)_getch();
                } break;
            case 48: case 49: case 50: case 51: case 52: case 53: case 54: case 55: case 56: case 57:
            case 44: case 45:
            {
                string editBuffer = "";
                editBuffer += static_cast<char>(key);
                string newValue = editingValue(0, currentRow, currentCol);
                if (!newValue.empty()) 
                        matrix[currentRow][currentCol] = stod(newValue);
            }
            break;
            }
        }
    }
    return matrix;
}
//вывод матрицы
void print_matrix(const vector<vector<double>>& matrix) {
    int row = matrix.size(), col = matrix[0].size();
    bool edit = true;
    printColor("|-", 0x0D);
    for (int i = 0; i < col; i++) {
        printColor("----------", 0x0D);
        if (i < col - 1) printColor("-|-", 0x0D); //соединение, если не последняя колонка
    }
    printColor("-|\n", 0x0D);

    for (int i = 0; i < row; i++) {
        printColor("|", 0x0D);
        for (int j = 0; j < col; j++) {
            setColor(0x0F);
            cout << setw(10) << matrix[i][j];
            printColor("  |", 0x0D);
        }
        cout << endl;
        //между строк разделители
        if (i < row - 1) {
            printColor("|-", 0x0D);
            for (int j = 0; j < col; j++) {
                printColor("----------", 0x0D);
                if (j < col - 1) printColor("-|-", 0x0D);
            }
            printColor("-|\n", 0x0D);
        }
    }
    //нижняя граница
    printColor("|_", 0x0D);
    for (int i = 0; i < col; i++) {
        printColor("__________", 0x0D);
        if (i < col - 1) printColor("_|_", 0x0D);
    }
    printColor("_|\n", 0x0D);
}
//умножение матриц
vector<vector<double>> MultiplicationMatrix(const vector<vector<double>>& A, vector<vector<double>>& B) {
    int rowA = A.size(); //3
    int colA = A[0].size();//1
    int rowB = B.size();//1
    int colB = B[0].size();//4

    vector<vector<double>> ResultMatrix(rowA, vector<double>(colB, 0.0));

    for (int i = 0; i < rowA; i++) {//3
        for (int j = 0; j < colB; j++) { //4
            for (int k = 0; k < colA; k++) {//1
                ResultMatrix[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    return ResultMatrix;
}
//сложение матриц
vector<vector<double>> MatrixAddition(const vector<vector<double>>& A, const vector<vector<double>>& B) {
    int rowA, colA, rowB, colB;
    rowA = A.size();
    colA = A[0].size();
    rowB = B.size();
    colB = B[0].size();

    vector<vector<double>> ResultMatrixAddition(rowA, vector<double>(colA, 0.0));
    
    for (int i = 0; i < rowA; i++) {
        for (int j = 0; j < colA; j++) {
            ResultMatrixAddition[i][j] = A[i][j] + B[i][j];
        }
    }
    return ResultMatrixAddition;
}
//вычитание матриц
vector<vector<double>> MatrixSubtraction(const vector<vector<double>>& A, const vector<vector<double>>& B) {
    int rowA, colA, rowB, colB;
    rowA = A.size();//2
    colA = A[0].size();//3
    rowB = B.size();//2
    colB = B[0].size();//3

    vector<vector<double>> ResultMatrixSubtraction(rowA, vector<double>(colA, 0.0));
   
    for (int i = 0; i < rowA; i++) {
        for (int j = 0; j < colA; j++) {
            ResultMatrixSubtraction[i][j] = A[i][j] - B[i][j];
        }
    }
    
    return ResultMatrixSubtraction;
}
//Транспонирование матриц
vector<vector<double>> MatrixTransposition(const vector<vector<double>>& A) {
    int rowA = A.size(), colA = A[0].size();//2 * 3
    vector<vector<double>> ResultMatrixTransposition(colA, vector<double>(rowA));
    for (int i = 0; i < colA; i++) { //3
        for (int j = 0; j < rowA; j++) { //2 
            ResultMatrixTransposition[i][j] = A[j][i];
        }
    }
    return ResultMatrixTransposition;
}
//Умножение на число
vector<vector<double>> MultiplicationNumber(vector<vector<double>>& N) {
    int rowN = N.size(), colN = N[0].size();
    int num;
    printColor("Введите число, на которое хотите умножить матрицу?\n", 0x0B);
    cin >> num;
    vector<vector<double>> ResultMultiplicationNumber(rowN, vector<double>(colN));
    for (int i = 0; i < rowN; i++) {
        for (int j = 0; j < colN; j++)
            ResultMultiplicationNumber[i][j] = N[i][j] * num;
    }
    return ResultMultiplicationNumber;
}

void menuMatrix(vector<vector<double>>& A, vector<vector<double>>& B) {
    int rowA = A.size(), colA = A[0].size();
    int rowB = B.size(), colB = B[0].size();
    if (rowA == rowB && colA == colB && colA != rowB) {
        printColor("\nВыберите действие:\n", 0x0B);
        printColor("1. Сложение матриц \n", 0x0A);
        printColor("2. Вычитание матриц\n", 0x0A);
        printColor("3. Транспонирование матриц\n", 0x0A);
        printColor("4. Умножение матрицы на скаляр\n", 0x0A);
        printColor("5. Прекратить действия\n", 0x0A);
    }
    else if (rowA == rowB && colA == colB && colA == rowB) {
        printColor("\nВыберите действие:\n", 0x0B);
        printColor("1. Сложение матриц \n", 0x0A);
        printColor("2. Вычитание матриц\n", 0x0A);
        printColor("3. Умножение матриц\n", 0x0A);
        printColor("4. Транспонирование матриц\n", 0x0A);
        printColor("5. Умножение матрицы на скаляр\n", 0x0A);
        printColor("6. Прекратить действия\n", 0x0A);
    }
    else if (colA == rowB) {
        printColor("\nВыберите действие:\n", 0x0B);
        printColor("1. Умножение матриц\n", 0x0A);
        printColor("2. Транспонирование матриц\n", 0x0A);
        printColor("3. Умножение матрицы на скаляр\n", 0x0A);
        printColor("4. Прекратить действия\n", 0x0A);
    }
    else {
        printColor("\nВыберите действие:\n", 0x0B);
        printColor("1. Транспонирование матриц\n", 0x0A);
        printColor("2. Умножение матрицы на скаляр\n", 0x0A);
        printColor("3. Прекратить действия\n", 0x0A);
    }
}


int main() {
    setlocale(LC_ALL, "RU");
    cout << "ДЕЙСТВИЯ С МАТРИЦАМИ\n\n";
    int num;
    setColor(0x0F);
 
    vector<vector<double>> matrixA = getUserMatrix("\nСоздание матрицы А\n");
    cout << endl;
    clearScreen();

    vector<vector<double>> matrixB = getUserMatrix("Создание матрицы B\n");
    cout << endl;
    clearScreen();

    cout << setw(10) << left;
    printColor("Ваши матрицы\n", 0x0D);
    printColor("Матрица А\n", 0x0B);
    print_matrix(matrixA);
    cout << endl;
    printColor("Матрица B\n", 0x0D);
    print_matrix(matrixB);
    cout << endl;

    int rowA = matrixA.size(), colA = matrixA[0].size();
    int rowB = matrixB.size(), colB = matrixB[0].size();

    if (rowA == rowB && colA == colB && colA != rowB) {
        do {
            menuMatrix(matrixA, matrixB);
            cout << ">> ";
            cin >> num;
            switch (num) {
            case(1): {
                vector<vector<double>> MatrixAdditionResult = MatrixAddition(matrixA, matrixB);
                printColor("\nРезультат сложения матриц\n", 0x0E);
                print_matrix(MatrixAdditionResult);
                break;
            }
            case(2): {
                vector<vector<double>> MatrixSubtractionResult = MatrixSubtraction(matrixA, matrixB);
                printColor("\nРезультат вычитания матриц\n", 0x0E);
                print_matrix(MatrixSubtractionResult);
                cout << endl;
                break;
            }
            case(3): {
                vector<vector<double>> MatrixTranspositionResult1 = MatrixTransposition(matrixA);
                vector<vector<double>> MatrixTranspositionResult2 = MatrixTransposition(matrixB);
                printColor("\nРезультат транспонирования матрицы А\n", 0x0A);
                print_matrix(MatrixTranspositionResult1);
                printColor("\nРезультат транспонирования матрицы B\n", 0x0A);
                print_matrix(MatrixTranspositionResult2);
                break;
            }
            case (4): {
                char name;
                printColor("Какую матрицу будете умножать?(A\\B)\n", 0x0A);
                cin >> name;
                if (name == 'A') {
                    vector<vector<double>> MultiplicationNumberResult1 = MultiplicationNumber(matrixA);
                    printColor("Результат умножения матрицы A на числo\n", 0x0B);
                    print_matrix(MultiplicationNumberResult1);
                }
                else if (name == 'B') {
                    vector<vector<double>> MultiplicationNumberResult2 = MultiplicationNumber(matrixB);
                    printColor("Результат умножения матрицы В на числo\n", 0x0B);
                    print_matrix(MultiplicationNumberResult2);
                }
                else
                    printColor("Такой матрицы не существует!\n", 0x0C);
                break;
            }
            case(5): {
                printColor("\nДо свидания!\n", 0x0A);
                break;
            }
            default: {
                printColor("Неверный выбор!", 0x0C);
                break;
            }
            }
        } while (num != 5);
    }
    else if (rowA == rowB && colA == colB && colA == rowB) {
        do {
            menuMatrix(matrixA, matrixB);
            cout << ">> ";
            cin >> num;
            switch (num) {
            case(1): {
                vector<vector<double>> MatrixAdditionResult = MatrixAddition(matrixA, matrixB);
                printColor("\nРезультат сложения матриц\n", 0x0B);
                print_matrix(MatrixAdditionResult);
                break;
            }
            case(2): {
                vector<vector<double>> MatrixSubtractionResult = MatrixSubtraction(matrixA, matrixB);
                printColor("\nРезультат вычитания матриц\n", 0x0B);
                print_matrix(MatrixSubtractionResult);
                cout << endl;
                break;
            }
            case(3): {
                vector<vector<double>> MulMatrixResult = MultiplicationMatrix(matrixA, matrixB);
                printColor("\nРезультат умножения матриц\n", 0x0B);
                print_matrix(MulMatrixResult);
                break;
            }
            case(4): {
                vector<vector<double>> MatrixTranspositionResult1 = MatrixTransposition(matrixA);
                vector<vector<double>> MatrixTranspositionResult2 = MatrixTransposition(matrixB);
                printColor("\nРезультат транспонирования матрицы А\n", 0x0B);
                print_matrix(MatrixTranspositionResult1);
                printColor("\nРезультат транспонирования матрицы B\n", 0x0B);
                print_matrix(MatrixTranspositionResult2);
                break;
            }
            case (5): {
                char name;
                printColor("Какую матрицу будете умножать?(A\\B)\n", 0x0B);
                cin >> name;
                if (name == 'A') {
                    vector<vector<double>> MultiplicationNumberResult1 = MultiplicationNumber(matrixA);
                    printColor("Результат умножения матрицы A на числo\n", 0x0B);
                    print_matrix(MultiplicationNumberResult1);
                }
                else if (name == 'B') {
                    vector<vector<double>> MultiplicationNumberResult2 = MultiplicationNumber(matrixB);
                    printColor("Результат умножения матрицы В на числo\n", 0x0E3);
                    print_matrix(MultiplicationNumberResult2);
                }
                else
                    printColor("Такой матрицы не существует!\n", 0x0C);
                break;
            }
            case(6): {
                printColor("\nДо свидания!\n", 0x0A);
                break;
            }
            default: {
                printColor("Неверный выбор!", 0x0C);
                break;
            }
            }
        } while (num !=6);
    }
    else if (colA == rowB) {
        do {
            menuMatrix(matrixA, matrixB);
            cout << ">> ";
            cin >> num;
            switch (num) {
            case(1): {
                vector<vector<double>> MulMatrixResult = MultiplicationMatrix(matrixA, matrixB);
                printColor("\nРезультат умножения матриц\n", 0x0B);
                print_matrix(MulMatrixResult);
                break;
            }
            case(2): {
                vector<vector<double>> MatrixTranspositionResult1 = MatrixTransposition(matrixA);
                vector<vector<double>> MatrixTranspositionResult2 = MatrixTransposition(matrixB);
                printColor("\nРезультат транспонирования матрицы А\n", 0x0B);
                print_matrix(MatrixTranspositionResult1);
                printColor("\nРезультат транспонирования матрицы B\n", 0x0B);
                print_matrix(MatrixTranspositionResult2);
                break;
            }
            case (3): {
                char name;
                printColor("Какую матрицу будете умножать?(A\\B)\n", 0x0B);
                cin >> name;
                if (name == 'A') {
                    vector<vector<double>> MultiplicationNumberResult1 = MultiplicationNumber(matrixA);
                    printColor("Результат умножения матрицы A на числo\n", 0x0B);
                    print_matrix(MultiplicationNumberResult1);
                }
                else if (name == 'B') {
                    vector<vector<double>> MultiplicationNumberResult2 = MultiplicationNumber(matrixB);
                    printColor("Результат умножения матрицы В на числo\n", 0x0B);
                    print_matrix(MultiplicationNumberResult2);
                }
                else
                    printColor("Такой матрицы не существует!\n", 0x0C);
                break;
            }
            case(4): {
                printColor("\nДо свидания!\n", 0x0A);
                break;
            }
            default: {
                printColor("Неверный выбор!", 0x0C);
                break;
            }
            }

        } while (num != 4);
    }
    else {
        do {
            menuMatrix(matrixA, matrixB);
            cout << ">> ";
            cin >> num;
            switch (num) {
            case(1): {
                vector<vector<double>> MatrixTranspositionResult1 = MatrixTransposition(matrixA);
                vector<vector<double>> MatrixTranspositionResult2 = MatrixTransposition(matrixB);
                printColor("\nРезультат транспонирования матрицы А\n", 0x0B);
                print_matrix(MatrixTranspositionResult1);
                printColor("\nРезультат транспонирования матрицы B\n", 0x0B);
                print_matrix(MatrixTranspositionResult2);
                break;
            }
            case (2): {
                char name;
                printColor("Какую матрицу будете умножать?(A\\B)\n", 0x0B);
                cin >> name;
                if (name == 'A') {
                    vector<vector<double>> MultiplicationNumberResult1 = MultiplicationNumber(matrixA);
                    printColor("Результат умножения матрицы A на числo\n", 0x0B);
                    print_matrix(MultiplicationNumberResult1);
                }
                else if (name == 'B') {
                    vector<vector<double>> MultiplicationNumberResult2 = MultiplicationNumber(matrixB);
                    printColor("Результат умножения матрицы В на числo\n", 0x0B);
                    print_matrix(MultiplicationNumberResult2);
                }
                else
                    printColor("Такой матрицы не существует!\n", 0x0C);
                break;
            }
            case(3): {
                printColor("\nДо свидания!\n", 0x0A);
                break;
            }
            default: {
                printColor("Неверный выбор!", 0x0C);
                break;
            }
            }

        } while (num != 3);
    }
    return 0;
}
