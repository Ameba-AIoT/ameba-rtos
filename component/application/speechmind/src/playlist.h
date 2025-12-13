#ifndef _SPEECH_MIND_PLAYLIST
#define _SPEECH_MIND_PLAYLIST

typedef struct Playlist Playlist;
void Playlist_addSong(Playlist* playlist, const char* title);
void Playlist_clear(Playlist* playlist);
void Playlist_display(Playlist* playlist);
int Playlist_size(Playlist* playlist);

char* Playlist_getFirstSongTitle(Playlist* playlist);
int Playlist_removeFirstSong(Playlist* playlist);

Playlist* Playlist_create(void);
void Playlist_destroy(Playlist* playlist);


#endif /* _SPEECH_MIND_PLAYLIST */