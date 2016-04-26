<?php

namespace Drupal\paragraphs_extra\Widget\Plugins;

use \Drupal\paragraphs_extra\Widget\WidgetBase;

class Preview extends WidgetBase {

  public function element(array &$element, array $paragraphs) {
    // Since the array of paragraphs to be rendered may contain multiple
    // paragraphs that have not yet been saved, we can't just call entity_view
    // on the entire array because the paragraphs view callback uses the entity
    // id to differentiate between entities in the render array.
    foreach ($paragraphs as $delta => $paragraph) {
      $element[$delta] = entity_view('paragraphs_item', array($paragraph));
    }
  }
}
