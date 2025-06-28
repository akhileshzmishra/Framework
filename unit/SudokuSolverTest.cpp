#include <gtest/gtest.h>
#include <SodukuSolver.h>
#include <memory>
#include <string>
#include <type_traits>
#include <CommonTestUtils.hpp>
using namespace sudoku;//


TEST (SudokuSolverTest, SolveOneSudoku) {
    std::vector<std::vector<char>> board = {
        {'5','3','.','.','7','.','.','.','.'},
        {'6','.','.','1','9','5','.','.','.'},
        {'.','9','8','.','.','.','.','6','.'},
        {'8','.','.','.','6','.','.','.','3'},
        {'4','.','.','8','.','3','.','.','1'},
        {'7','.','.','.','2','.','.','.','6'},
        {'.','6','.','.','.','.','2','8','.'},
        {'.','.','.','4','1','9','.','.','5'},
        {'.','.','.','.','8','.','.','7','9'}};
    sudoku::Board d_board(board);
    d_board.solve();

    for (const auto& row: board) {
        for (const auto& cell: row) {
            EXPECT_NE(cell, '.');
        }
    }
}

TEST (SudokuSolverTest, SolveTwoSudoku) {
    std::vector<std::vector<char>> board = {
        {'6','.','.','.','.','.','.','.','.'},
        {'.','.','1','.','.','.','4','6','.'},
        {'5','.','.','.','.','3','2','.','.'},
        {'.','.','.','.','8','.','.','.','.'},
        {'.','9','6','4','.','.','.','.','.'},
        {'2','.','4','.','1','.','5','.','3'},
        {'.',',','8','.','.','.','.','3','.'},
        {'.','.','3','2','4','.','.','1','.'},
        {'.','.','.','.','.','8','6','.','2'}};
    sudoku::Board d_board(board);
    d_board.solve();

    for (const auto& row: board) {
        for (const auto& cell: row) {
            EXPECT_NE(cell, '.');
        }
    }
}

TEST (SudokuSolverTest, solveThirdSudoku) {
    std::vector<std::vector<char>> board = {
        {'.','.','.','.','6','.','.','2','9'},
        {'7','.','.','.','8','.','.','.','.'},
        {'.','.','2','.','.','1','.','.','.'},
        {'4','3','7','.','.','.','.','.','.'},
        {'.','1','.','.','.','.','.','.','5'},
        {'.','8','.','.','.','.','6','.','1'},
        {'.',',','8','7','.','.','3','.','.'},
        {'.','.','.','3','.','4','.','1','.'},
        {'.','.','.','.','.','.','4','.','.'}};
    sudoku::Board d_board(board);
    d_board.solve();

    for (const auto& row: board) {
        for (const auto& cell: row) {
            EXPECT_NE(cell, '.');
            std::cout << cell << ",";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}