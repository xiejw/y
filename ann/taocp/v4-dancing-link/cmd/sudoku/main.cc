#include <vector>

#include <algos/dal.h>

#include <eve/base/log.h>

#define SIZE 9

#ifndef DEBUG
#define DEBUG 1
#endif

#ifndef PID  // Problem ID
#define PID 1
#endif

// clang-format off
static const int PROLBEMS[][SIZE * SIZE] = {
    // From The Art of Computer Programming, Vol 4, Dancing Links.
    {
        0, 0, 3,  0, 1, 0,  0, 0, 0,
        4, 1, 5,  0, 0, 0,  0, 9, 0,
        2, 0, 6,  5, 0, 0,  3, 0, 0,

        5, 0, 0,  0, 8, 0,  0, 0, 9,
        0, 7, 0,  9, 0, 0,  0, 3, 2,
        0, 3, 8,  0, 0, 4,  0, 6, 0,

        0, 0, 0,  2, 6, 0,  4, 0, 3,
        0, 0, 0,  3, 0, 0,  0, 0, 8,
        3, 2, 0,  0, 0, 7,  9, 5, 0,
    },

    // The World's Hardest Sudoku.
    //
    // https://gizmodo.com/can-you-solve-the-10-hardest-logic-puzzles-ever-created-1064112665
    {
        8, 0, 0,  0, 0, 0,  0, 0, 0,
        0, 0, 3,  6, 0, 0,  0, 0, 0,
        0, 7, 0,  0, 9, 0,  2, 0, 0,

        0, 5, 0,  0, 0, 7,  0, 0, 0,
        0, 0, 0,  0, 4, 5,  7, 0, 0,
        0, 0, 0,  1, 0, 0,  0, 3, 0,

        0, 0, 1,  0, 0, 0,  0, 6, 8,
        0, 0, 8,  5, 0, 0,  0, 1, 0,
        0, 9, 0,  0, 0, 0,  4, 0, 0,
    },
};
// clang-format on

// -----------------------------------------------------------------------------
// prototypes.
//
// -----------------------------------------------------------------------------
struct option {
    int x;
    int y;
    int k;
};

static void   printProblem( const int *problem );
static size_t searchOptions( const int *problem, std::vector<option> &options );
static void getColHeadId( int i, int j, int k, size_t *p, size_t *r, size_t *c,
                          size_t *b );
//
//
// -----------------------------------------------------------------------------
// main.
//
// -----------------------------------------------------------------------------
int
main( )
{
    // -------------------------------------------------------------
    // Select and print problem
    const int *problem = PROLBEMS[PID];
    logInfo( "Problem:\n" );
    printProblem( problem );

    // -------------------------------------------------------------
    // Search options.
    std::vector<option> options{ };
    options.reserve( 9 * 9 * 9 );  // at most 9^3 options.
    const size_t options_count = searchOptions( problem, options );

    if ( DEBUG ) {
        logInfo( "total options count %zu\n", options_count );
        logInfo( "top 10 options:\n" );
        for ( size_t i = 0; i < 10 && i < options_count; i++ ) {
            logInfo( "  x %d, y %d, k %d\n", options[i].x, options[i].y,
                     options[i].k );
        }
    }

    // -------------------------------------------------------------
    // Step 1: Prepare dancing links table.
    //
    // For each column head (item) and option, we need 4 columns to cover.  See
    // getColHeadId for details. The extra one is the header.

    algos::dal::Table t{ /*n_col_heads=*/4 * 81,
                         /*n_options_total=*/4 * options_count };

    //
    // -------------------------------------------------------------
    // Step 2: Hide all column heads which have been filled by the problem
    // already.
    size_t item_ids[4];
    for ( int x = 0; x < SIZE; x++ ) {
        int offset = x * SIZE;
        for ( int y = 0; y < SIZE; y++ ) {
            int num = problem[offset + y];
            if ( num == 0 ) continue;

            getColHeadId( x, y, num,
                          /*p=*/item_ids,
                          /*r=*/item_ids + 1,
                          /*c=*/item_ids + 2,
                          /*b=*/item_ids + 3 );
            t.CoverCol( item_ids[0] );
            t.CoverCol( item_ids[1] );
            t.CoverCol( item_ids[2] );
            t.CoverCol( item_ids[3] );
        }
    }

    //
    // -------------------------------------------------------------
    // Step 3: Append options to the dancing links table;
    for ( size_t i = 0; i < options_count; i++ ) {
        auto o = &options[i];
        getColHeadId( o->x, o->y, o->k, /*p=*/item_ids,
                      /*r=*/item_ids + 1,
                      /*c=*/item_ids + 2,
                      /*b=*/item_ids + 3 );
        t.AppendOption( item_ids, o );
    }
    //
    //         //
    //         -------------------------------------------------------------
    //         vec_t(size_t) sols = vecNew();
    //         vecReserve(&sols, SIZE * SIZE);
    //
    //         if (dalSearchSolution(t, sols)) {
    //                 // Dup the problem as we are going to modify it.
    //                 int solution[SIZE * SIZE];
    //                 memcpy(solution, problem, sizeof(int) * SIZE * SIZE);
    //
    //                 logInfo("Found solution:\n");
    //                 size_t n = vecSize(sols);
    //                 for (size_t i = 0; i < n; i++) {
    //                         struct option *o             = dalNodeData(t,
    //                         sols[i]); solution[o->x * SIZE + o->y] =
    //                         o->k;
    //                 }
    //                 printProblem(solution);
    //
    //         } else {
    //                 printf("No solution.\n");
    //         }
    //
    return 0;
}

//
// -----------------------------------------------------------------------------
// helper methods.
//
// -----------------------------------------------------------------------------

// print the Sudoku Problem on screen.
void
printProblem( const int *problem )
{
    // header
    printf( "+-----+-----+-----+\n" );
    for ( int x = 0; x < SIZE; x++ ) {
        int offset = x * SIZE;
        printf( "|" );
        for ( int y = 0; y < SIZE; y++ ) {
            int num = problem[offset + y];
            if ( num == 0 )
                printf( " " );
            else
                printf( "%d", problem[offset + y] );

            if ( ( y + 1 ) % 3 != 0 )
                printf( " " );
            else
                printf( "|" );
        }
        printf( "\n" );
        if ( ( x + 1 ) % 3 == 0 ) printf( "+-----+-----+-----+\n" );
    }
}

#define POS( x, y ) ( ( x ) * SIZE + ( y ) )

// Seach all options that on (x,y) the digit k is allowed to be put there.
//
// The argument options must have enough capacity to hold all potential
// options
size_t
searchOptions( const int *problem, std::vector<option> &options )
{
    size_t total = 0;

    for ( int x = 0; x < SIZE; x++ ) {
        const int offset = x * SIZE;

        for ( int y = 0; y < SIZE; y++ ) {
            if ( problem[offset + y] > 0 ) continue;  // prefilled.

            int box_x = ( x / 3 ) * 3;
            int box_y = ( y / 3 ) * 3;

            for ( int k = 1; k <= SIZE; k++ ) {
                // Search row
                for ( int j = 0; j < SIZE; j++ ) {
                    if ( problem[j + offset] == k ) {
                        goto not_a_option;
                    }
                }

                // Search column
                for ( int j = 0; j < SIZE; j++ ) {
                    if ( problem[POS( j, y )] == k ) {
                        goto not_a_option;
                    }
                }

                // Search box. static unroll
                if ( problem[POS( box_x, box_y )] == k ||
                     problem[POS( box_x, box_y + 1 )] == k ||
                     problem[POS( box_x, box_y + 2 )] == k ||
                     problem[POS( box_x + 1, box_y )] == k ||
                     problem[POS( box_x + 1, box_y + 1 )] == k ||
                     problem[POS( box_x + 1, box_y + 2 )] == k ||
                     problem[POS( box_x + 2, box_y )] == k ||
                     problem[POS( box_x + 2, box_y + 1 )] == k ||
                     problem[POS( box_x + 2, box_y + 2 )] == k ) {
                    goto not_a_option;
                }

                options.push_back( { .x = x, .y = y, .k = k } );
                total++;
            not_a_option:
                (void)0;
            }
        }
    }
    return total;
}

// For a digit k (1-based) in cell (i,j), generats four ids (1-based) for
// the following four col heads:
//
// - p{i,j} // pos
// - r{i,k} // row with digit
// - c{j,k} // col with digit
// - b{x,k} // box with digit where x = 3 * floor(i/3) + floor(j/3).
void
getColHeadId( int i, int j, int k, size_t *p, size_t *r, size_t *c, size_t *b )
{
    int x      = 3 * ( i / 3 ) + ( j / 3 );
    int offset = 0;

    k = k - 1;  // k is 1 based.

    *p = (size_t)( i * SIZE + j + offset + 1 );  // item id is 1 based.
    offset += SIZE * SIZE + 1;

    *r = (size_t)( i * SIZE + k + offset );
    offset += SIZE * SIZE;

    *c = (size_t)( j * SIZE + k + offset );
    offset += SIZE * SIZE;

    *b = (size_t)( x * SIZE + k + offset );
}
