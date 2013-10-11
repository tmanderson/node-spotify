#ifndef _SEARCH_RESULT_H
#define _SEARCH_RESULT_H

#include <libspotify/api.h>
#include <memory>
#include <vector>
#include <string>

#include "Track.h"
#include "Album.h"
#include "Playlist.h"
#include "Artist.h"

class Search {
friend class NodeSearch;
public:
  Search(sp_search* _search);
  Search(const Search& other) : search(other.search), didYouMeanText(other.didYouMeanText), link(other.link) {
    sp_search_add_ref(search);
  };
  ~Search() {
    sp_search_release(search);
  };
  std::vector<std::shared_ptr<Track>> getTracks();
  std::vector<std::shared_ptr<Album>> getAlbums();
  std::vector<std::shared_ptr<Artist>> getArtists();
  std::vector<std::shared_ptr<Playlist>> getPlaylists();
  void execute(std::string query, int trackOffset, int trackLimit,
    int albumOffset, int albumLimit,
    int artistOffset, int artistLimit,
    int playlistOffset, int playlistLimit);
private:
  sp_search* search;
  std::string didYouMeanText;
  std::string link;
};

#endif