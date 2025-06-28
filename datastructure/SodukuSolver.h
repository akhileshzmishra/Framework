//
// Created by Akhilesh Mishra on 29/01/2025.
//

#ifndef SODUKUSOLVER_H
#define SODUKUSOLVER_H
#include <array>
#include <thread>
#include <vector>
#include <unordered_map>
#include <iostream>

namespace sudoku{

static const std::unordered_map<unsigned long, int> OPTION_TO_INDEX= {
    {1<<1, 1},{1<<2, 2},{1<<3, 3},{1<<4, 4},{1<<5, 5},
    {1<<6, 6},{1<<7, 7},{1<<8, 8},{1<<9, 9}
 };

class Options{
    /**
     * @brief
     */
    std::bitset<10> d_bitset{0};
 public:
    [[nodiscard]] bool set(int digit){
        if (withinBounds(digit)){
            d_bitset[digit] = true;
            return true;
        }
        return false;
    }

    [[nodiscard]] bool unSet(int digit){
        if (withinBounds(digit)){
            d_bitset[digit] = false;
            return true;
        }
        return false;
    }

    void reset(){
        d_bitset.reset();
    }

    void setAllOptions(){
        d_bitset.set();
    }

    [[nodiscard]] bool isSet(int digit) const{
        if (withinBounds(digit)){
            return d_bitset.test(digit);
        }
        return false;
    }

    static bool withinBounds(int i) {
        return ((i >= 1) && (i <= 9));
    }

    [[nodiscard]] size_t numberOption() const{
        return d_bitset.count() - d_bitset.test(0);
    }

    [[nodiscard]] int option() const{
        if (numberOption() > 1){
            throw std::runtime_error("option is more than one.");
        }
        auto numberToFind = d_bitset.to_ullong() - d_bitset.test(0);
        const auto& o = OPTION_TO_INDEX.find(numberToFind);
        if (o == OPTION_TO_INDEX.end()){
            throw std::runtime_error("option is more than one.");
        }
        return o->second;
    }

    [[nodiscard]] int unSetOption() const{
        if (numberOption() != 8) {
            throw std::runtime_error("left over option is more than one.");
        }
        for (int i = 1; i <= 9; i++){
            if (d_bitset.test(i)){
                return i;
            }
        }
        throw std::runtime_error("option is more than one.");
    }

 };

 inline std::ostream& operator << (std::ostream& ostr, const Options& o)
 {
    std::string val(11, '_');
    for (int i = 1; i <= 9; i++){
        if (o.isSet(i)){
            val[i - 1] = std::to_string(i)[0];
        }
    }
    ostr << val;
    return ostr;
 }

class DigitBalance
{
    std::array<Options, 9> d_options;
 public:
    [[nodiscard]] bool insert(int index, int digit){
        if (isWithinBounds(index)){
            return d_options[index].set(digit);
        }
        return false;
    }

    [[nodiscard]] bool remove(int index, int digit){
        if (isWithinBounds(index)){
            return d_options[index].unSet(digit);
        }
        return false;
    }

    [[nodiscard]] bool contains(int index, int option) const{
        if (isWithinBounds(index)){
            return d_options[index].isSet(option);
        }
        return false;
    }
    [[nodiscard]] size_t totalFill(int index) const{
        if (isWithinBounds(index)){
            return d_options[index].numberOption();
        }
        return -1;
    }
    [[nodiscard]] Options getOption(int i) const {
        if (isWithinBounds(i)){
            return d_options[i];
        }
        return {};
    }

    [[nodiscard]] int option(int index) const{
        if (isWithinBounds(index) && totalFill(index) == 8){
            return d_options[index].unSetOption();
        }
        return -1;
    }
private:
    static bool isWithinBounds(int index) {
        return (index >= 0 && index < 9);
    }
};

inline std::ostream& operator << (std::ostream& ostr, const DigitBalance& o){
    for (int i = 0; i < 9; i++){
        ostr << o.getOption(i) << ", ";
    }
    ostr << std::endl;
    return ostr;
}

class SquareBalance
{
    std::array<std::array<Options, 3>, 3> d_options;
public:
    SquareBalance() = default;

    bool insert(int row, int col, int digit){
        auto [r, c] = fromBoardRowsAndCols(row, col);
        if (isWithinBounds(r, c)){
            return d_options[r][c].set(digit);
        }
        return false;
    }

    bool remove(int row, int col, int digit){
        auto [r, c] = fromBoardRowsAndCols(row, col);
        if (isWithinBounds(r, c)){
            return d_options[r][c].unSet(digit);
        }
        return false;
    }

    [[nodiscard]] bool contains(int row, int col, int digit) const{
        auto [r, c] = fromBoardRowsAndCols(row, col);
        if (isWithinBounds(r, c)){
            return d_options[r][c].isSet(digit);
        }
        return false;
    }
    [[nodiscard]] size_t totalFill(int row, int col) const{
        auto [r, c] = fromBoardRowsAndCols(row, col);
        if (isWithinBounds(r, c)){
            return d_options[r][c].numberOption();
        }
        return 0;
    }

    [[nodiscard]] Options getOption(int r, int c) const{
        return d_options[r][c];
    }

    [[nodiscard]] size_t option(int row, int col) const{
        auto [r, c] = fromBoardRowsAndCols(row, col);
        if (isWithinBounds(r, c)){
            return d_options[r][c].unSetOption();
        }
        return 0;
    }
private:
    static std::pair<int, int> fromBoardRowsAndCols(int row, int col){
        return {row/3, col/3};
    }
    static bool isWithinBounds(int r, int c){
        return (r >= 0 && c >= 0 && r < 3 && c < 3);
    }
};

 inline std::ostream& operator << (std::ostream& ostr, const SquareBalance& o)
 {
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            ostr << o.getOption(i, j) << ", ";
        }
        ostr << std::endl;
    }
    return ostr;
 }

class BoardNode
{
    char d_concrete = 0;
    Options d_options;
public:
    [[nodiscard]] bool setConcrete(int option){
        if (option >= 1 && option <= 9){
            d_concrete = std::to_string(option)[0];
            d_options.reset();
            return d_options.set(option);
        }

        return false;
    }
    [[nodiscard]] bool isConcrete() const{
        return d_concrete != 0;
    }

    [[nodiscard]] char concrete() const{
        return d_concrete;
    }

    [[nodiscard]] bool set(int digit){
        if (!isConcrete()){
            return d_options.set(digit);
        }
        return false;
    }

    [[nodiscard]] bool unSet(int digit){
        if (!isConcrete()){
            return d_options.unSet(digit);
        }
        return false;
    }

    void reset(){
        if (!isConcrete()){
            d_options.reset();
        }
    }

    void setAllOptions(){
        if (!isConcrete()){
            d_options.setAllOptions();
        }
    }

    [[nodiscard]] bool isSet(int Option) const{
        return d_options.isSet(Option);
    }

    [[nodiscard]] size_t numberOption() const{
        if (!isConcrete()){
            return d_options.numberOption();
        }
        return -20;
    }

    [[nodiscard]] int option() const{
        if (!isConcrete()){
            return d_options.option();
        }
        return -1;
    }
};

inline std::ostream& operator << (std::ostream& ostr, const BoardNode& o){
    std::string val(11, '_');
    if (o.isConcrete()){
        val[o.concrete() - '0'] = o.concrete();
        val[o.concrete() - '0' + 1] = 'C';
    }
    else {
        for (int i = 1; i <= 9; i++){
            if (o.isSet(i)){
                val[i - 1] = std::to_string(i)[0];
            }
        }
    }
    ostr << val;
    return ostr;
}

inline std::ostream& operator << (std::ostream& ostr, const std::array<std::array<BoardNode, 9>, 9>& o){
    for (size_t i = 0; i < o.size(); i++){
        for (size_t j = 0; j < o.size(); j++){
            ostr << std::right << o[i][j] << ", ";
        }
        ostr << std::endl;
    }
    ostr << std::endl;
    return ostr;
}


template<std::convertible_to<char> T = char, class Container = std::vector<std::vector<T>>>
class Board{
    std::array<std::array<BoardNode, 9>, 9> d_Board;
    Container& d_outBoard;
    DigitBalance d_rowBank;
    DigitBalance d_colBank;
    SquareBalance d_subSqBank;
    int d_numberOfDots = 0;
public:
    explicit Board(Container& board): d_outBoard(board){}

    void solve(){
        initialize();
        int number = 0;;
        while(d_numberOfDots > 0 && number < 50){
            relax();
            uniqueInRow();
            uniqueCols();
            uniqueSquare();
            number++;
        }
        if (d_numberOfDots > 0) {
            bool suceeded = false;
            recurseForAnswers(0,0, suceeded);
            std::cout << "suceeded=" << suceeded << std::endl;
        }
        std::cout << "------------" << std::endl;
        std::cout << d_Board << std::endl;
        std::cout << d_rowBank << std::endl;
        std::cout << d_colBank << std::endl;
        std::cout << d_subSqBank << std::endl;
    }
private:
    void initialize(){
        for (int i = 0; i < 9; i++){
            for (int j = 0; j < 9; j++){
                char element = d_outBoard[i][j];
                if (element != '.'){
                    int number = element - '0';
                    (void)d_Board[i][j].setConcrete(static_cast<int>(number));
                    (void)d_rowBank.insert(i, number);
                    (void)d_colBank.insert(j, number);
                    d_subSqBank.insert(i, j, number);
                }
                else{
                    d_numberOfDots++;
                }
            }
        }

        setOptionsInBoard();
    }

    void recurseForAnswers(int r, int c, bool& succeeded){

        if (r == 9 || c == 9){
            succeeded = true;
            return;
        }

        if (d_Board[r][c].isConcrete()){
            if (c == 8){
                recurseForAnswers(r + 1, 0, succeeded);
            }
            else {
                recurseForAnswers(r, c + 1, succeeded);
            }
            return;
        }

        for (int num = 1; num <= 9; num++){
            if (d_rowBank.contains(r, num) || d_colBank.contains(c, num) || d_subSqBank.contains(r, c, num)){
                continue;
            }
            (void)d_rowBank.insert(r, num);
            (void)d_colBank.insert(c, num);
            d_subSqBank.insert(r, c, num);
            d_outBoard[r][c] = std::to_string(num)[0];
            if (c == 8){
                recurseForAnswers(r + 1, 0, succeeded);
                if (succeeded) return;
            }
            else {
                recurseForAnswers(r, c + 1, succeeded);
                if (succeeded) return;
            }

            (void)d_rowBank.remove(r, num);
            (void)d_colBank.remove(c, num);
            d_subSqBank.remove(r, c, num);
        }
    }


    void setOptionsInBoard(){
        for (int i = 0; i < 9; i++){
            for (int j = 0; j < 9; j++){
                if (d_Board[i][j].isConcrete()){
                    continue;
                }
                d_Board[i][j].reset();
                for (int num = 1; num <= 9; num++){
                    if (d_rowBank.contains(i, num) || d_colBank.contains(j, num) || d_subSqBank.contains(i, j, num)){
                        continue;
                    }

                    (void)d_Board[i][j].set(num);

                }
            }
        }
    }

    void relax(){
        for (int i = 0; i < 9; i++){
            for (int j = 0; j < 9; j++){
                if (d_Board[i][j].isConcrete()){
                    continue;
                }
                if (d_Board[i][j].numberOption() == 1){
                    int option = d_Board[i][j].option();
                    (void)fill(i, j, option);
                }
            }
        }
    }

    void uniqueInRow(){
        for (int i = 0; i < 9; i++){
            int countArray[10] = {0,};
            for (int num = 1; num <= 9; num++){
                for (int j = 0; j < 9; j++){
                    if (d_Board[i][j].isConcrete()){
                        continue;
                    }
                    if (d_Board[i][j].isSet(num)){
                        countArray[num]++;
                    }
                }
            }

            for (int num = 1; num <= 9; num++){
                if (countArray[num] == 1){
                    for (int j = 0; j < 9; j++){
                        if (d_Board[i][j].isSet(num)){
                            (void)fill(i, j, num);
                        }
                    }
                }
            }
        }
    }

    void uniqueCols(){
        for (int j = 0; j < 9; j++){
            int countArray[10] = {0,};
            for (int num = 1; num <= 9; num++){
                for (int i = 0; i < 9; i++){
                    if (d_Board[i][j].isConcrete()){
                        continue;
                    }
                    if (d_Board[i][j].isSet(num)){
                        countArray[num]++;
                    }
                }
            }

            for (int num = 1; num <= 9; num++){
                if (countArray[num] == 1){
                    for (int i = 0; i < 9; i++){
                        if (d_Board[i][j].isSet(num)){
                            (void)fill(i, j, num);
                        }
                    }
                }
            }
        }
    }

    void uniqueSquare(){
        for (int usR = 0; usR < 3; usR++){
            for (int usC = 0; usC < 3; usC++){
                int rStart = usR*3;
                int cStart = usC*3;
                int countArray[10] = {0,};
                for (int num = 1; num <= 9; num++){
                    for (int i = rStart; i < rStart + 3; i++){
                        for (int j = cStart; j < cStart + 3; j++){
                            if (d_Board[i][j].isSet(num)){
                                countArray[num]++;
                            }
                        }
                    }
                }

                for (int num = 1; num <= 9; num++){
                    if (countArray[num] == 1){
                        for (int i = rStart; i < rStart + 3; i++){
                            for (int j = cStart; j < cStart + 3; j++){
                                if (d_Board[i][j].isSet(num)){
                                    (void)fill(i, j, num);
                                }
                            }
                        }
                    }
                }

            }
        }
    }

    bool fill(int r, int c, int option ){
        if (option < 1 || option > 9) {
            return false;
        }

        if (d_rowBank.contains(r, option)){
            return false;;
        }

        if (d_colBank.contains(c, option)){
            return false;
        }

        if (d_subSqBank.contains(r, c, option)){
            return false;;
        }

        (void)d_Board[r][c].setConcrete(option);

        for (auto& options: d_Board[c]){
            (void)options.unSet(option);
        }
        (void)d_rowBank.insert(r, option);

        for (auto& options: d_Board[r]){
            (void)options.unSet(option);
        }
        (void)d_colBank.insert(c, option);

        unSetSquares(r, c, option);

        d_outBoard[r][c] = std::to_string(option)[0];
        d_numberOfDots--;
        setOptionsInBoard();
        return true;
    }

    void unSetSquares(int r, int c, int option)
    {
        d_subSqBank.insert(r, c, option);
        int rStart = r/3;
        int cStart = c/3;
        for (int i = rStart; i < rStart + 3; i++){
            for (int j = cStart; j < cStart + 3; j++){
                (void)d_Board[i][j].unSet(option);
            }
        }
    }
};


}

#endif //SODUKUSOLVER_H
