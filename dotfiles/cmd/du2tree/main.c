#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
        char         *name;
        long long     size;
        struct Node **children;
        int           child_count;
        int           child_cap;
} Node;

long long
parse_size( const char *s )
{
        double val  = atof( s );
        char   unit = s[strlen( s ) - 1];
        switch ( toupper( unit ) ) {
        case 'P':
                val *= 1024LL * 1024 * 1024 * 1024 * 1024;
                break;
        case 'T':
                val *= 1024LL * 1024 * 1024 * 1024;
                break;
        case 'G':
                val *= 1024LL * 1024 * 1024;
                break;
        case 'M':
                val *= 1024LL * 1024;
                break;
        case 'K':
                val *= 1024LL;
                break;
        default:
                break;
        }
        return (long long)val;
}

char *
hrsize( long long size, char *buf )
{
        const char *units[] = { "B", "K", "M", "G", "T", "P" };
        double      s       = size;
        int         u       = 0;
        while ( s >= 1024 && u < 5 ) {
                s /= 1024;
                u++;
        }
        if ( u == 0 )
                sprintf( buf, "%lld%s", size, units[u] );
        else
                sprintf( buf, "%.1f%s", s, units[u] );
        return buf;
}

Node *
new_node( const char *name, long long size )
{
        Node *n = calloc( 1, sizeof( Node ) );
        n->name = strdup( name );
        n->size = size;
        return n;
}

Node *
find_or_create( Node *parent, const char *name )
{
        for ( int i = 0; i < parent->child_count; i++ ) {
                if ( strcmp( parent->children[i]->name, name ) == 0 )
                        return parent->children[i];
        }
        if ( parent->child_count == parent->child_cap ) {
                parent->child_cap =
                    parent->child_cap ? parent->child_cap * 2 : 4;
                parent->children = realloc(
                    parent->children, parent->child_cap * sizeof( Node * ) );
        }
        Node *n                                 = new_node( name, 0 );
        parent->children[parent->child_count++] = n;
        return n;
}

int
cmp_nodes( const void *a, const void *b )
{
        Node *na = *(Node **)a, *nb = *(Node **)b;
        if ( nb->size == na->size ) return strcmp( na->name, nb->name );
        return ( nb->size > na->size ) - ( nb->size < na->size );
}

void
sort_children( Node *n )
{
        for ( int i = 0; i < n->child_count; i++ )
                sort_children( n->children[i] );
        qsort( n->children, n->child_count, sizeof( Node * ), cmp_nodes );
}

void
print_tree( Node *n, int depth, int *has_more, int has_more_len , int max_depth)
{
        if (max_depth > 0 && depth > max_depth) return;
        char buf[64];
        printf( "%8s ", hrsize( n->size, buf ) );

        for ( int i = 0; i < has_more_len - 1; i++ ) {
                printf( "%s", has_more[i] ? "│   " : "    " );
        }
        if ( has_more_len > 0 )
                printf( "%s── ", has_more[has_more_len - 1] ? "├" : "└" );

        printf( "%s\n", n->name );

        for ( int i = 0; i < n->child_count; i++ ) {
                int more[128];
                memcpy( more, has_more, has_more_len * sizeof( int ) );
                more[has_more_len] = ( i < n->child_count - 1 );
                print_tree( n->children[i], depth + 1, more, has_more_len + 1 , max_depth);
        }
}

void
insert_path( Node *root, char *path, long long size )
{
        char *tok = strtok( path, "/" );
        Node *cur = root;
        while ( tok ) {
                Node *child = find_or_create( cur, tok );
                if ( !strchr( path, '/' ) )  // last component
                        child->size = size;
                cur = child;
                tok = strtok( NULL, "/" );
        }
}

int
main( int argc, char**argv )
{
        int max_depth = -1; // -1 == no limit
        if (argc == 3 && strcmp(argv[1], "-d") == 0) {
                max_depth = atoi(argv[2]);
        }

        Node *root = new_node( ".", 0 );
        char  line[1024];

        while ( fgets( line, sizeof( line ), stdin ) ) {
                char size_str[64], path[960];
                if ( sscanf( line, "%63s %959[^\n]", size_str, path ) == 2 ) {
                        long long size = parse_size( size_str );
                        char     *p    = path;
                        if ( strncmp( p, "./", 2 ) == 0 ) p += 2;
                        insert_path( root, strdup( p ), size );
                }
        }

        sort_children( root );
        for ( int i = 0; i < root->child_count; i++ ) {
                int more[1] = { i < root->child_count - 1 };
                print_tree( root->children[i], 1, more, 1, max_depth);
        }

        return 0;
}
