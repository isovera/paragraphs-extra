<?php

namespace Drupal\paragraphs_extra\Widget;

class State {
  protected $element;
  protected $parents;
  protected $field_name;
  protected $form_state;
  protected $values;
  protected $input;
  protected $field_state;
  protected $items;

  public function __construct(array $element, array &$form_state) {
    // Setup the state getter / setter parameters.
    $this->element = $element;
    $this->parents = $element['#parents'];
    $this->field_name = $element['#field_name'];

    // Store the current widget values for manupulation.
    $this->form_state = &$form_state;
    $this->values = drupal_array_get_nested_value($this->form_state['values'], $this->parents);
    $this->input = drupal_array_get_nested_value($this->form_state['input'], $this->parents);
    $this->field_state = field_form_get_state($element['#field_parents'], $this->field_name, $element['#language'], $this->form_state);

    // Fill in the canonical item list.
    $this->items = array();
    for ($delta = 0; $delta < $this->field_state['items_count']; $delta++) {
      // Check if the entity is in the input or values array already.
      if (isset($this->values[$delta])) {
        $this->items[$delta] = $this->values[$delta];
      }
      else if (isset($this->input[$delta])) {
        $this->items[$delta] = $this->input[$delta];
      }

      // If the entity is missing from any of the states fill it in.
      if ($this->field_state['entity'][$delta]) {
        if (!isset($this->values[$delta]['entity'])) {
          $this->values[$delta]['entity'] = $this->field_state['entity'][$delta];
        }
        if (!isset($this->values[$delta]['entity'])) {
          $this->input[$delta]['entity'] = $this->field_state['entity'][$delta];
        }
        if (!isset($this->items[$delta]['entity'])) {
          $this->items[$delta]['entity'] = $this->field_state['entity'][$delta];
        }
      }

      // Ensures all form values associated with elements from a
      // field_attach_form call are appropriately populated in the form state
      // for each entity.
      $this->assignParagraph($this->items[$delta], $this->items[$delta]['entity']);
    }

    // Allow widgets to discard changes.
    $this->discard = FALSE;
  }

  public function getItems() {
    return $this->items;
  }

  public function discard() {
    $this->discard = TRUE;
  }

  public function clear() {
    foreach ($this->items as $delta => $item) {
      $this->remove($delta);
    }
  }

  public function insert($paragraph, $weight = NULL) {
    // Find the first available paragraphs item that can be used.
    $delta = $this->getNextItem($paragraph->bundle);
    $this->assignParagraph($this->items[$delta], $paragraph);

    // Override the weight if applicable
    if (isset($weight)) {
      $this->items[$delta]['_weight'] = $weight;
    }

    // Mark that the inserted entity as non-deleted.
    $this->items[$delta]['entity']->removed = FALSE;
    $this->items[$delta]['entity']->confirmed_removed = FALSE;
    return $delta;
  }

  public function &get($delta) {
    if (isset($this->items[$delta])) {
      return $this->items[$delta];
    }
    else {
      return NULL;
    }
  }

  public function remove($delta) {
    // Mark the entity for deletion.
    if (isset($this->items[$delta]['entity'])) {
      $this->items[$delta]['entity']->removed = TRUE;
      $this->items[$delta]['entity']->confirmed_removed = TRUE;
    }
  }

  public function commit() {
    if (!$this->discard) {
      // Sort items be weight.
      uasort($this->items, '_field_sort_items_helper');

      // Re-order the items with valid weights and update the values array.
      $weight = -1 * $this->field_state['items_count'] + 1;
      $delta = 0;
      foreach ($this->items as $item) {
        if ($item) {
          $item['_weight'] = $weight++;
          $this->input[$delta] = $item;
          $this->values[$delta] = $item;
          $this->field_state['entity'][$delta] = $item['entity'];
          ++$delta;
        }
      }
      field_form_set_state($this->element['#field_parents'], $this->field_name, $this->element['#language'], $this->form_state, $this->field_state);
    }

    // Update the form values.
    form_set_value($this->element, $this->values, $this->form_state);
    drupal_array_set_nested_value($this->form_state['input'], $this->parents, $this->input, TRUE);
  }

  protected function getNextItem($bundle) {
    $delta = -1;

    // See if there is an existing item marked for deletion that can be re-used.
    foreach ($this->items as $delta => $item) {
      if (isset($item['entity']) && $this->isRemoved($item['entity']) && $item['entity']->bundle == $bundle) {
        return $delta;
      }
    }

    // Otherwise create a new item.
    $this->items[++$delta] = array(
      '_weight' => $delta,
      'entity' => entity_create('paragraphs_item', array(
        'bundle' => $bundle,
        'field_name' => $this->field_name,
      )),
    );
    ++$this->field_state['items_count'];

    return $delta;
  }

  protected function assignParagraph(&$item, $paragraph) {
    $wrapper = entity_metadata_wrapper('paragraphs_item', $paragraph);
    $instances = field_info_instances('paragraphs_item', $paragraph->bundle);
    if ($instances) {
      foreach ($instances as $field_name => $info) {
        if (isset($paragraph->$field_name)) {
          // Set the field value for the entity that will be saved.
          $item['entity']->$field_name = $paragraph->$field_name;

          // This is required to keep the values for the form elements added by
          // field_attach_form up to date (as opposed to just the entities
          // themselves.
          if ($wrapper) {
            $item[$field_name] = $wrapper->$field_name->value();
          }
        }
      }
    }
  }

  protected function isRemoved($paragraph) {
    return isset($paragraph->removed) && isset($paragraph->confirmed_removed)
      && $paragraph->removed && $paragraph->confirmed_removed;
  }
}

