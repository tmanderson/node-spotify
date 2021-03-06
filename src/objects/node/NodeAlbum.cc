#include "NodeAlbum.h"
#include "NodeTrack.h"
#include "NodeArtist.h"
#include "../spotify/Track.h"

NodeAlbum::NodeAlbum(std::unique_ptr<Album> _album) : album(std::move(_album)) {
  album->nodeObject = this;
};

NodeAlbum::~NodeAlbum() {
  if(album->nodeObject == this) {
    album->nodeObject = nullptr;
  }
}

NAN_GETTER(NodeAlbum::getName) {
  Nan::HandleScope scope;
  NodeAlbum* nodeAlbum = Nan::ObjectWrap::Unwrap<NodeAlbum>(info.This());
  info.GetReturnValue().Set(Nan::New<String>(nodeAlbum->album->name().c_str()).ToLocalChecked());
}

NAN_GETTER(NodeAlbum::getLink) {
  Nan::HandleScope scope;
  NodeAlbum* nodeAlbum = Nan::ObjectWrap::Unwrap<NodeAlbum>(info.This());
  info.GetReturnValue().Set(Nan::New<String>(nodeAlbum->album->link().c_str()).ToLocalChecked());
}

NAN_METHOD(NodeAlbum::getCoverBase64) {
  Nan::HandleScope scope;
  NodeAlbum* nodeAlbum = Nan::ObjectWrap::Unwrap<NodeAlbum>(info.This());
  info.GetReturnValue().Set(Nan::New<String>(nodeAlbum->album->coverBase64().c_str()).ToLocalChecked());
}

NAN_METHOD(NodeAlbum::browse) {
  Nan::HandleScope scope;
  NodeAlbum* nodeAlbum = Nan::ObjectWrap::Unwrap<NodeAlbum>(info.This());
  if(nodeAlbum->album->albumBrowse == nullptr) {
    nodeAlbum->makePersistent();
    nodeAlbum->browseCompleteCallback = std::unique_ptr<Nan::Callback>(new Nan::Callback(info[0].As<Function>()));

    //Mutate the V8 object.
    Handle<Object> nodeAlbumV8 = nodeAlbum->handle();
    Nan::SetAccessor(nodeAlbumV8, Nan::New<String>("tracks").ToLocalChecked(), getTracks);
    Nan::SetAccessor(nodeAlbumV8, Nan::New<String>("review").ToLocalChecked(), getReview);
    Nan::SetAccessor(nodeAlbumV8, Nan::New<String>("copyrights").ToLocalChecked(), getCopyrights);
    Nan::SetAccessor(nodeAlbumV8, Nan::New<String>("artist").ToLocalChecked(), getArtist);

    nodeAlbum->album->browse();
  } else {
    nodeAlbum->callBrowseComplete();
  }
  return;
}

NAN_GETTER(NodeAlbum::getTracks) {
  Nan::HandleScope scope;
  NodeAlbum* nodeAlbum = Nan::ObjectWrap::Unwrap<NodeAlbum>(info.This());
  std::vector<std::shared_ptr<Track>> tracks = nodeAlbum->album->tracks();
  Handle<Array> nodeTracks = Nan::New<Array>(tracks.size());
  for(int i = 0; i < (int)tracks.size(); i++) {
    NodeTrack* nodeTrack = new NodeTrack(tracks[i]);
    nodeTracks->Set(Nan::New<Number>(i), nodeTrack->createInstance());
  }
  info.GetReturnValue().Set(nodeTracks);
}

NAN_GETTER(NodeAlbum::getReview) {
  Nan::HandleScope scope;
  NodeAlbum* nodeAlbum = Nan::ObjectWrap::Unwrap<NodeAlbum>(info.This());
  info.GetReturnValue().Set(Nan::New<String>(nodeAlbum->album->review().c_str()).ToLocalChecked());
}

NAN_GETTER(NodeAlbum::getCopyrights) {
  Nan::HandleScope scope;
  NodeAlbum* nodeAlbum = Nan::ObjectWrap::Unwrap<NodeAlbum>(info.This());
  std::vector<std::string> copyrights = nodeAlbum->album->copyrights();
  Handle<Array> nodeCopyrights = Nan::New<Array>(copyrights.size());
  for(int i = 0; i < (int)copyrights.size(); i++) {
    nodeCopyrights->Set(Nan::New<Number>(i), Nan::New<String>(copyrights[i].c_str()).ToLocalChecked());
  }
  info.GetReturnValue().Set(nodeCopyrights);
}

NAN_GETTER(NodeAlbum::getArtist) {
  Nan::HandleScope scope;
  NodeAlbum* nodeAlbum = Nan::ObjectWrap::Unwrap<NodeAlbum>(info.This());
  NodeArtist* nodeArtist = new NodeArtist(nodeAlbum->album->artist());
  info.GetReturnValue().Set(nodeArtist->createInstance());
}

NAN_GETTER(NodeAlbum::isLoaded) {
  Nan::HandleScope scope;
  NodeAlbum* nodeAlbum = Nan::ObjectWrap::Unwrap<NodeAlbum>(info.This());
  info.GetReturnValue().Set(Nan::New<Boolean>(nodeAlbum->album->isLoaded()));
}

void NodeAlbum::init() {
  Nan::HandleScope scope;
  Handle<FunctionTemplate> constructorTemplate = NodeWrapped::init("Album");
  Nan::SetAccessor(constructorTemplate->InstanceTemplate(), Nan::New<String>("name").ToLocalChecked(), getName);
  Nan::SetAccessor(constructorTemplate->InstanceTemplate(), Nan::New<String>("link").ToLocalChecked(), getLink);
  Nan::SetAccessor(constructorTemplate->InstanceTemplate(), Nan::New<String>("isLoaded").ToLocalChecked(), isLoaded);
  Nan::SetPrototypeMethod(constructorTemplate, "getCoverBase64", getCoverBase64);
  Nan::SetPrototypeMethod(constructorTemplate, "browse", browse);
  NodeAlbum::constructorTemplate.Reset(constructorTemplate);
}
